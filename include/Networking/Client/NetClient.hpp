// NetClient.hpp
#pragma once

#include "Networking/NetworkConstants.hpp"
#include "Networking/Packets/PacketBase.hpp"

#include "Networking/Packets/PacketFactory.hpp"
#include "Networking/PeerData.hpp"

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

    bool is_connected() {return peer.isConnected();}

    void pollEvents();
    void connectServer();
    void push_outgoing_packet(std::unique_ptr<PacketBase> packet, ENetPacketFlag flag);
    int send_packet_queue();

    std::unique_ptr<PacketBase> popPacketQueue();


private:
    void sendPackets(const std::vector<uint8_t>& packet_content, const enet_uint8 channel, const ENetPacketFlag flag = ENET_PACKET_FLAG_RELIABLE);
    void on_connection();

    std::queue<std::unique_ptr<PacketBase>> packet_queue;
    std::queue<std::pair<std::unique_ptr<PacketBase>, ENetPacketFlag>> outgoing_packets;

    const char* server_address;
    int server_port;

    ENetHost* client;
    ENetAddress address;
    ENetEvent event;
    PeerData peer = PeerData();
};