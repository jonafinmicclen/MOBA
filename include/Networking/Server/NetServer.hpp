// NetServer.hpp
#pragma once

#include "Networking/NetworkConstants.hpp"
#include "Networking/Packets/PacketBase.hpp"

#include "Networking/Packets/PacketFactory.hpp"
#include "Networking/Packets/TestPacket.hpp"
#include "Networking/PeerData.hpp"

#include "Networking/Message/Message.hpp"

#include "Networking/PacketFlag.hpp"

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
    void broadcastPackets(const std::vector<uint8_t>& packet_content, const enet_uint8 channel, const ENetPacketFlag flag = ENET_PACKET_FLAG_RELIABLE);

    std::unique_ptr<PacketBase> popPacket(const PacketFlag flag);


    private:
    static constexpr uint8_t FLAG_TO_CHANNEL(PacketFlag flag) {
        switch(flag) {
            case PacketFlag::GAMEPLAY: return 0;
            case PacketFlag::MOVEMENT: return 1;
            case PacketFlag::CHAT:     return 2;
        }
    }

    std::vector<std::shared_ptr<PeerData>> connected_clients;

    uint8_t next_client_id = 0;

    std::queue<std::unique_ptr<PacketBase>> gameplay_queue;
    std::queue<std::unique_ptr<PacketBase>> movement_queue;
    std::queue<std::unique_ptr<PacketBase>> chat_queue;

    ENetAddress address;
    ENetHost* server;
    ENetEvent event;

    const int max_clients;

    std::queue<std::unique_ptr<PacketBase>>* getQueueFromFlag(const PacketFlag flag);
    void initialiseClientConnection(ENetPeer* peer);
    void handleRecieve(ENetEvent event);
};