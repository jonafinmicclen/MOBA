// NetClient.hpp
#pragma once

#include "Networking/NetworkConstants.hpp"
#include "Networking/Packets/PacketBase.hpp"

#include "Networking/Packets/PacketFactory.hpp"
#include "Networking/PeerData.hpp"
#include "Networking/Message/Message.hpp"
#include "Networking/Message/MessageHeader.hpp"
#include "Networking/Packets/PacketDistributor.hpp"

#include <enet/enet.h>

#include <iostream>
#include <format>
#include <vector>
#include <queue>
#include <memory>


class NetClient {
public:

    NetClient(const char* server_add, int port);
    ~NetClient();

    bool is_connected() {return peer_data->isConnected();}

    void pollEvents();
    void connectServer();
    void push_outgoing_packet(Message message);
    int send_packet_queue();

    std::unique_ptr<PacketBase> popPacketQueue();


private:
    void sendPackets(const std::vector<uint8_t>& packet_content, const enet_uint8 channel, const ENetPacketFlag flag = ENET_PACKET_FLAG_RELIABLE);
    void on_connection(ENetPeer* peer);

    std::queue<std::unique_ptr<PacketBase>> packet_queue;
    std::queue<Message> outgoing_messages;

    const char* server_address;
    int server_port;

    ENetHost* client;
    ENetAddress address;
    ENetEvent event;
    std::unique_ptr<PeerData> peer_data;
};