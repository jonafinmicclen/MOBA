#pragma once

#include "Game/Control/PlayerCommand.hpp"
#include "Server/ServerWorld.hpp"
#include "Common/Memory/FIFOQueue.hpp"
#include "Common/Memory/BiMap.hpp"
#include "Common/Debug/debug.hpp"
#include "Game/Components/Transform.hpp"
#include <cassert>
#include "Game/Packets/Gameplay/ClientCommandPacket.hpp"
#include "Core/Networking/Session/PeerDirectory.hpp"

#include "Core/Networking/Packets/PacketDistributor.hpp"
#include "Common/Memory/FIFOQueue.hpp"

#include "Game/Components/Navigation/Path.hpp"
#include "Game/AI/Pathfinding.hpp"
#include "Game/MapDef.hpp"

#include "Core/Adapter/NetAdapter.hpp"
#include "Game/Packets/Gameplay/SpawnPacket.hpp"
#include "Game/Components/Stats/MovementSpeed.hpp"


using ClientInputQueue = FIFOQueue<ClientInput>;

class ClientInputSystem {
public:
    ClientInputSystem(PacketDistributor& distributor, PeerAccountMap& peer_dir, NetAdapter& networker, bool practice_mode)
    : peer_dir_(peer_dir), networker_(networker), practice_mode_(practice_mode) {
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

    void update(ServerWorld& world, BiMap<AccountHash, EntityHandle> acc_entity_map, MapDef& map) {
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

            switch (c->command) {
                case ClientCommand::SPAWN_DUMMY:
                    if (practice_mode_) {
                        spawnDummy(world, *handle, c->mouse_pos);
                    }
                    break;

                case ClientCommand::MOVE: {
                    Path* p = world.tryGet<Path>(*handle);
                    if (p == nullptr) {
                        DEBUG_LOG("No path found when applying command");
                        return;
                    }
                    Transform* t = world.tryGet<Transform>(*handle);
                    if (t == nullptr) {
                        DEBUG_LOG("No transform found when applying command");
                        return;
                    }

                    WorldSpacePos pos { t->position.x, t->position.y };
                    Pathfinding::aStar(map, pos, c->mouse_pos, *p);
                    break;
                }
            }
        }
    }

private:
    ClientInputQueue queue_;
    PeerAccountMap& peer_dir_;
    NetAdapter& networker_;
    bool practice_mode_;

    // Spawns a stationary, unowned dummy Naren - on the opposite team to
    // whoever pressed T - at the given world position, and broadcasts it to
    // every connected client. Practice mode only (see GameArgs::
    // practice_mode) - dummies get no EntityOwnershipPacket since no
    // account controls them.
    void spawnDummy(ServerWorld& world, EntityHandle requester, WorldSpacePos location) {
        Team dummy_team{0};
        if (Team* requester_team = world.tryGet<Team>(requester)) {
            dummy_team.team = requester_team->team == 0 ? 1 : 0;
        }

        Transform spawn_transform;
        spawn_transform.position.x = location.x;
        spawn_transform.position.y = location.y;

        Path p;
        SpawnPoint spawn_point;
        spawn_point.point = location;
        spawn_point.team = dummy_team;
        MovementSpeed ms;
        ms.speed = 0.03f;

        EntityHandle handle = world.add<ServerArchetypeId::Champion>(
            spawn_transform, p, dummy_team, spawn_point, ms
        );

        SpawnCommand cmd;
        cmd.entity = "Naren";
        cmd.position = spawn_transform;
        cmd.server_handle = handle;
        SpawnPacket pkt;
        pkt.setData(cmd);
        networker_.sendPacket(&pkt, Channel::RELIABLECOMMANDS, {});

        DEBUG_LOG("Spawned dummy Naren (team " << (int)dummy_team.team << ") at " << location.x << ", " << location.y);
    }

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