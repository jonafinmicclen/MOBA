// NetServer.hpp
#pragma once

#include "Networking/NetworkConstants.hpp"
#include "Networking/Packets/PacketBase.hpp"

#include "Networking/Packets/PacketFactory.hpp"

#include <enet/enet.h>

#include <format>
#include <iostream>
#include <vector>
#include <memory>
#include <queue>


class NetServer {
    public:

    NetServer(const int max_client, const enet_uint32 host = ENET_HOST_ANY, const enet_uint16 port = 1234);
    ~NetServer();

    void pollEvents();
    void sendPackets(const std::vector<uint8_t>& packet_content, ENetPeer* target_peer, const enet_uint8 channel, ENetPacketFlag const flag = ENET_PACKET_FLAG_RELIABLE);
    void broadcastPackets(const PacketBase* packet, const enet_uint8 channel, const ENetPacketFlag flag = ENET_PACKET_FLAG_RELIABLE);


    std::unique_ptr<PacketBase> popQueue();


    private:

    void handleIncomingPacket(ENetPacket* packet);
    void initialiseClientConnection();

    std::vector<ENetPeer*> connected_clients;
    uint8_t next_client_id = 0;
    std::queue<std::unique_ptr<PacketBase>> packet_queue;

    ENetAddress address;
    ENetHost* server;
    ENetEvent event;

    const int max_clients;
    
};