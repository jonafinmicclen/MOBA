#include "Networking/Core/Networker.hpp"


Networker::Networker(const NetConfig& cfg) : config_(cfg) {

    if (enet_initialize () != 0) {
        throw std::runtime_error("Failed to initialize ENet");
    }

    ENetAddress* address = new ENetAddress;
    if (cfg.address) {
        enet_address_set_host(address, cfg.address);
        address->port = cfg.port;
    } else {
        delete address;
        address = nullptr;
    }
    
    networker_ = enet_host_create(address, cfg.max_connections, cfg.num_channels, 0, 0);

    if (!networker_) {
        enet_deinitialize();
        throw std::runtime_error("Failed to create ENet server");
    }
}

Networker::~Networker() {
    stop();
    if (networker_) {
        enet_host_destroy(networker_);
        networker_ = nullptr;
    }
    enet_deinitialize();
}


void Networker::start() {
    if (thread_.joinable()) return;
    thread_ = std::jthread([this](std::stop_token st) {
        loop(st);
    });
}

void Networker::stop() {
    if (thread_.joinable()) {
        thread_.request_stop();
    }
}

void Networker::loop(std::stop_token st) {
    while (!st.stop_requested()) {
        pollCommands();
        pollEvents();
        sendQueues();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void Networker::sendQueues() {
    while (auto message = outgoing_queue_.popQueue()) {

        DEBUG_LOG("Message sent");

        bool queued = false;

        enet_uint8 channel = message->channel_idx;
        ENetPacket* packet = enet_packet_create(
            message->bytes.data(),
            message->bytes.size(),
            message->flags
        );

        // If many peers listed send to all
        for (auto pID : message->peers) {
            auto peer = peer_map_.find(pID);
            if (!peer) {
                continue;
            }
            if (enet_peer_send(*peer, channel, packet) == 0) {
                queued = true;
            }
        } 

        // If none assume broadcast
        if (message->peers.empty()) {
            enet_host_broadcast(networker_, channel, packet);
            queued = true;
        }

        if (!queued) {
            enet_packet_destroy(packet);
        }
    }
    enet_host_flush(networker_);
}

void Networker::pollCommands() {
    while (auto cmd = command_queue_.popQueue()) {
        std::visit([&](auto&& c) {
            using T = std::decay_t<decltype(c)>;

            if constexpr (std::is_same_v<T, ConnectCmd>) {
                ENetAddress addr{};
                if (enet_address_set_host(&addr, c.host.c_str()) != 0) {
                    DEBUG_LOG("Failed to resolve host: " << c.host);
                    return;
                }
                addr.port = c.port;

                ENetPeer* peer = enet_host_connect(networker_, &addr,
                                                   static_cast<enet_uint32>(config_.num_channels),
                                                   0);
                if (!peer) {
                    DEBUG_LOG("enet_host_connect failed");
                    return;
                }

                DEBUG_LOG("Connect requested to " << c.host << ":" << c.port);
            }
            else if constexpr (std::is_same_v<T, DisconnectCmd>) {
                DEBUG_LOG("Disconnet command not implemented");
            }
        }, *cmd);
    }
}
void Networker::pollEvents() {
    while (enet_host_service (networker_, &event_, 0) > 0) {
        switch (event_.type) {
            case ENET_EVENT_TYPE_CONNECT: onConnect(event_); break;
            case ENET_EVENT_TYPE_RECEIVE: onRecieve(event_); break;
            case ENET_EVENT_TYPE_DISCONNECT: onDisconnect(event_); break;
        }
    }
}

void Networker::onRecieve(const ENetEvent& e) {
    ENetPacket* packet = e.packet;
    
    std::vector<uint8_t> data(
        packet->data,
        packet->data + packet->dataLength
    );

    NetMessage message(
        static_cast<ChannelIDX>(e.channelID),
        0,
        data,
        {peer_map_.at(e.peer)}
    );

    incoming_queue_.pushQueue(std::move(message));

    DEBUG_LOG("Message recieved");

    onRecieveCompute(e);
    enet_packet_destroy(e.packet);
}

void Networker::onConnect(const ENetEvent& e) {
    peer_map_.insert(e.peer);
    event_queue_.pushQueue(NetConnectEvent(peer_map_.at(e.peer)));
    onConnectCompute(e);
}

void Networker::onDisconnect(const ENetEvent& e) {
    event_queue_.pushQueue(NetDisconnectEvent(peer_map_.at(e.peer)));
    peer_map_.erase(e.peer);
    onDisconnectCompute(e);
    e.peer->data = nullptr;
}

void Networker::pushMessage(NetMessage&& msg) {
    outgoing_queue_.pushQueue(std::move(msg));
}

void Networker::pushCommand(NetCommand&& cmd) {
    command_queue_.pushQueue(std::move(cmd));
}

std::optional<NetEvent> Networker::popEvent() {
    return event_queue_.popQueue();
}

std::optional<NetMessage> Networker::popMessage() {
    return incoming_queue_.popQueue();
}