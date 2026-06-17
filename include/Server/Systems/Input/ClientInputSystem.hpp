#pragma once

#include "Game/Control/PlayerCommand.hpp"
#include "Server/ServerWorld.hpp"
#include "Common/Memory/FIFOQueue.hpp"
#include "Common/Memory/BiMap.hpp"
#include "Common/Debug/debug.hpp"
#include "Game/Components/Transform.hpp"
#include <cassert>
#include "Game/Packets/Gameplay/ClientCommandPacket.hpp"
#include "Networking/Session/PeerDirectory.hpp"

#include "Networking/Packets/PacketDistributor.hpp"
#include "Common/Memory/FIFOQueue.hpp"


using ClientInputQueue = FIFOQueue<ClientInput>;

class ClientInputSystem {
public:
    ClientInputSystem(PacketDistributor& distributor, PeerAccountMap& peer_dir) 
    : peer_dir_(peer_dir) {
        AutoRegisterPacket<
            ClientInputPacket,
            PacketType::ClientInputPacket
        >::register_pkt();

        distributor.on<ClientInputPacket>(PacketType::ClientInputPacket,
            [this](const ClientInputPacket& p, const PacketMetadata& m){
                handlePacket(p, m);
            }
        );
    }

    void update(ServerWorld& world, BiMap<AccountHash, EntityHandle> acc_entity_map) {
        while (auto c = queue_.pop()) {
            auto& hash = c->account_hash;
            DEBUG_LOG(*hash);
            if(!hash) {
                DEBUG_LOG("hash not present");
                continue;
            }
            EntityHandle* handle = acc_entity_map.findByA(*hash);
            if (handle == nullptr) {
                DEBUG_LOG("Command recieved from non-assigned");
                return;
            }
            Transform* t = world.tryGet<Transform>(*handle);
            if (t == nullptr) {
                DEBUG_LOG("No transform found when applying command");
                return;
            }
            t->position.x = c->mouse_pos.x;
            t->position.y = c->mouse_pos.y;
        }       
    }

private:
    ClientInputQueue queue_;
    PeerAccountMap& peer_dir_;


    void handlePacket(const ClientInputPacket& p, const PacketMetadata& m) {
        if (m.ids.size() != 1) {
            DEBUG_LOG("Client input command received with invalid peer count");
            return;
        }

        ClientInput c = p.getData();
        PeerID peer = m.ids[0];
        auto acc_hash = peer_dir_.find(peer);

        if (!acc_hash) {
            DEBUG_LOG("Client input command recieved from unregistered peer");
            return;
        }

        c.account_hash = *acc_hash;
        queue_.push(c);
    }
};