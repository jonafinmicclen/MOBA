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
            break;
    
        case ENET_EVENT_TYPE_RECEIVE:

            if (event.packet) {
                auto new_packet = PacketFactory::packetFromBytes(event.packet->data, event.packet->dataLength);
                packet_queue.push(new_packet);
            }   

            enet_packet_destroy (event.packet);
            break;
        
        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout<<std::format("{} disconected.\n", event.peer->data);
            event.peer -> data = nullptr;
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

void NetServer::initialiseClientConnection() {
    // Send client id to client connected.
}

PacketBase* NetServer::popQueue() {
    if (packet_queue.empty()) return nullptr;
    PacketBase* packet = std::move(packet_queue.front());
    packet_queue.pop();
    return packet;
}