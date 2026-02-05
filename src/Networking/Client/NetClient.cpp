#include "Networking/Client/NetClient.hpp"


NetClient::NetClient(const char* server_add, int port) {

    server_address = server_add;
    server_port = port;

    client = enet_host_create (NULL, 1, NetConstants::NUM_CHANNELS, 0, 0);
 
    if (!client) {
        enet_deinitialize();
        throw std::runtime_error("Failed to create ENet client");
    }

}

NetClient::~NetClient() {
    if (client) {
        enet_host_destroy(client);
    }
}

void NetClient::pollEvents() {
    while (enet_host_service (client, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
            std::cout<<"Connection attempted to client which should have been already connected.";
            break;
    
        case ENET_EVENT_TYPE_RECEIVE:

            if (event.packet) {
                auto new_packet = PacketFactory::deserialisePacket(event.packet->data, event.packet->dataLength);
                packet_queue.push(std::move(new_packet));
            }   

            enet_packet_destroy (event.packet);
            break;
        
        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout<<std::format("{} disconected.\n", event.peer->data);
    
            event.peer -> data = NULL;
        }
    }
}

void NetClient::connectServer() {
    enet_address_set_host (& address, server_address);
    address.port = server_port;
    
    ENetPeer* peer = enet_host_connect(client, & address, 2, 0);    
    
    if (peer == nullptr) {
        std::cout<<"No available peers for initiating an ENet connection.\n";
        exit (EXIT_FAILURE);
    }
    
    if (enet_host_service (client, & event, 10000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::cout<<std::format("Connection to server @ {}:{} succeeded.\n", server_address, server_port);
        on_connection(peer);
    }
    else
    {
        enet_peer_reset (peer);
        std::cout<<std::format("Connection to server @ {}:{} failed.\n", server_address, server_port);

    }
}

void NetClient::sendPackets(const std::vector<uint8_t>& packet_content, const enet_uint8 channel, const ENetPacketFlag flag) {
    // Flag is defaulted
    ENetPacket* packet = enet_packet_create(
        packet_content.data(),
        packet_content.size(),
        flag
    );

    enet_peer_send (peer_data->getENetPeer(), channel, packet);
    enet_host_flush(client);
}

std::unique_ptr<PacketBase> NetClient::popPacketQueue() {
    if (packet_queue.empty()) return nullptr;
    auto packet = std::move(packet_queue.front());
    packet_queue.pop();
    return packet;
}

void NetClient::on_connection(ENetPeer* peer) {
    peer_data = std::make_unique<PeerData>(peer);
    peer_data->setID("Server");
}

void NetClient::push_outgoing_packet(Message message) {

    outgoing_messages.push(std::move(message));
}

int NetClient::send_packet_queue() {
    int n_sent = outgoing_messages.size();
    while (outgoing_messages.size()) {
        auto message = std::move(outgoing_messages.front());

        std::unique_ptr<PacketBase> packet = std::move(message.packet);
        uint8_t channel = static_cast<uint8_t>(message.header.flag);
        outgoing_messages.pop();
        auto bytes = packet->serialize();
        sendPackets(bytes, channel);
    }
    return n_sent;
}