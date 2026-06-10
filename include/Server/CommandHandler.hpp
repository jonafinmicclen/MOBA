#pragma once
#include <cassert>
#include "Networking/Packets/ClientCommandPacket.hpp"
#include "Networking/Session/PeerDirectory.hpp"

#include "Networking/Packets/PacketDistributor.hpp"
#include "Common/Memory/FIFOQueue.hpp"

using ClientCommandQueue = FIFOQueue<ClientCommand>;

class CommandHandler {
public:
    explicit CommandHandler(PacketDistributor& distributor, ClientCommandQueue& q, PeerDirectory& peer_dir) 
    : command_queue_(q), peer_dir_(peer_dir) {
        AutoRegisterPacket<
            ClientCommandPacket,
            PacketType::ClientCommandPacket
        >::register_pkt();

        distributor.on<ClientCommandPacket>(PacketType::ClientCommandPacket,
        [this](const ClientCommandPacket& p, const PacketMetadata& m){
            handleCommand(p, m);
        }
    );
    }

private:
    ClientCommandQueue& command_queue_;
    PeerDirectory& peer_dir_;

    void handleCommand(const ClientCommandPacket& p, const PacketMetadata& m) {
        if (m.ids.size() != 1) {
            DEBUG_LOG("Client input command received with invalid peer count");
            return;
        }

        ClientCommand c = p.getData();
        PeerID peer = m.ids[0];
        auto acc_hash = peer_dir_.find(peer);

        if (!acc_hash) {
            DEBUG_LOG("Client input command recieved from unregistered peer");
            return;
        }

        c.account_hash = *acc_hash;
        command_queue_.push(c);
    }
};