#include "Networking/Core/Networker.hpp"


Networker::Networker(const NetConfig& cfg) {
    if (enet_initialize () != 0) {
        throw std::runtime_error("Failed to initialize ENet");
    }

    ENetAddress address;
    enet_address_set_host(&address, cfg.address);
    address.port = cfg.port;
    networker_ = enet_host_create(&address, cfg.max_connections, cfg.num_channels, 0, 0);

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
        pollEvents();
        sendQueues();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void Networker::sendQueues() {
    while (auto message = outgoing_queue_.popQueue()) {

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

    onRecieveCompute(e);
    enet_packet_destroy(e.packet);
}

void Networker::onConnect(const ENetEvent& e) {
    peer_map_.insert(e.peer);
    onConnectCompute(e);
}

void Networker::onDisconnect(const ENetEvent& e) {
    peer_map_.erase(e.peer);
    onDisconnectCompute(e);
    e.peer->data = nullptr;
}

void Networker::pushMessage(NetMessage&& msg) {
    outgoing_queue_.pushQueue(std::move(msg));
}

std::optional<NetMessage> Networker::popMessage() {
    return incoming_queue_.popQueue();
}