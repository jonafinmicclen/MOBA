#include "Networking/Server/NetServer.hpp"


NetServer::NetServer(const int max_client, const enet_uint32 host, const enet_uint16 port) : max_clients(max_client) {

    connected_clients.reserve(max_client);

    address.host = host;
    address.port = port;

    if (enet_initialize () != 0)
    {
        throw std::runtime_error("Failed to initialize ENet");
    }

    server = enet_host_create(&address, max_clients, NetConstants::NUM_CHANNELS, 0, 0);

    if (!server) {
        enet_deinitialize();
        throw std::runtime_error("Failed to create ENet server");
    }
 
}

NetServer::~NetServer() {
    if (server) {
        enet_host_destroy(server);
        server = nullptr;
    }

    enet_deinitialize();
}

void NetServer::pollEvents() {
    while (enet_host_service (server, &event, 0) > 0)
    {
        switch (event.type)
        {
            case ENET_EVENT_TYPE_CONNECT:
                initialiseClientConnection(event.peer);
                break;
        
            case ENET_EVENT_TYPE_RECEIVE:
                handleRecieve(event);
                break;
            
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout<<std::format("{} disconected.\n", event.peer->data);
                event.peer -> data = nullptr;
            }
    }
}

void NetServer::handleRecieve(ENetEvent event) {
    if (event.packet) {
        auto new_packet = PacketFactory::deserialisePacket(event.packet->data, event.packet->dataLength);
        
        // Construct container
        auto message = std::make_unique<Message>();
        message->packet = std::move(new_packet);

        uint8_t id = id_by_peerptr.at(event.peer);
        message->header.id = id;

        enet_packet_destroy (event.packet);

        DEBUG_LOG("Packet stored to queue " << static_cast<int>(event.channelID));
        
        switch (static_cast<int>(event.channelID)) {
            case 1: // Movement
                movement_queue.push(std::move(message));
                break;

            case 0: // Gameplay
                gameplay_queue.push(std::move(message));
                break;

            case 2: // Chat
                chat_queue.push(std::move(message));
                break;
            default:
                DEBUG_LOG("Packed channelid missed");
        }
    }
}

void NetServer::broadcastPackets(const std::vector<uint8_t>& packet_content, const enet_uint8 channel, const ENetPacketFlag flag) {

    ENetPacket* packet = enet_packet_create(
        packet_content.data(),
        packet_content.size(),
        flag
    );

    enet_host_broadcast(server, channel, packet);
    enet_host_flush(server);
}

void NetServer::sendPackets(const std::vector<uint8_t>& packet_content, ENetPeer* target_peer, const enet_uint8 channel, ENetPacketFlag const flag) {

    ENetPacket* packet = enet_packet_create(
        packet_content.data(),
        packet_content.size(),
        flag
    );

    enet_peer_send (target_peer, channel, packet);
    enet_host_flush(server);
}

void NetServer::sendPackets(const PacketBase* packet, const uint8_t peer_id, const enet_uint8 channel, ENetPacketFlag const flag) {
    ENetPeer* peer = peerptr_by_id[peer_id];
    std::vector<uint8_t> packet_content = packet->serialize();
    sendPackets(packet_content, peer, channel, flag);
}

void NetServer::initialiseClientConnection(ENetPeer* peer) {
    auto peerData = std::make_shared<PeerData>(peer);
    connected_clients.push_back(peerData);
    peerptr_by_id[next_id] = peer;
    id_by_peerptr[peer] = next_id++;
    DEBUG_LOG("connection " << id_by_peerptr[peer] << "established");
}

std::unique_ptr<Message> NetServer::popPacket(const PacketFlag flag) {
    auto queue = getQueueFromFlag(flag);
    if (!queue) {
        DEBUG_LOG("No packets in queue");
        return nullptr;
    }
    if (!queue->size()) {
        return nullptr;
    }
    auto packet = std::move(queue->front());
    queue->pop();
    return packet;
}

std::queue<std::unique_ptr<Message>>* NetServer::getQueueFromFlag(const PacketFlag flag) {
    switch (flag) {
        case PacketFlag::GAMEPLAY:
            return &gameplay_queue;
        case PacketFlag::MOVEMENT:
            return &movement_queue;
        case PacketFlag::CHAT:
            return &chat_queue;
        default:
            DEBUG_LOG("Unrecognised flag");
            return nullptr;
    }
}

void NetServer::disconnectByID(uint8_t id) {
    DEBUG_LOG("Unimplemented");
    return;
}