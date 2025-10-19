// NetClient.hpp
#pragma once

#include "Networking/NetworkConstants.hpp"
#include "Networking/Packets/PacketBase.hpp"

#include "Networking/Packets/PacketFactory.hpp"

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

    void pollEvents();
    void connectServer();
    void sendPackets(const std::vector<uint8_t>& packet_content, const enet_uint8 channel, const ENetPacketFlag flag = ENET_PACKET_FLAG_RELIABLE);

    PacketBase* popQueue();


private:

    std::queue<PacketBase*> packet_queue;

    const char* server_address;
    int server_port;

    ENetHost* client;
    ENetAddress address;
    ENetPeer *peer;
    ENetEvent event;
};