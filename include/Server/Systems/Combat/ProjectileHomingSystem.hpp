#pragma once

#include <vector>

#include "Server/ServerWorld.hpp"
#include "Core/Adapter/NetAdapter.hpp"
#include "Game/Packets/Gameplay/DespawnPacket.hpp"

class ProjectileHomingSystem {
public:
    void update(ServerWorld& world, NetAdapter& net_adapter) {
        // Removing an entity mid-query would invalidate queryEntities'
        // iteration over the same archetype's storage (removeEntity does a
        // swap-remove) - collect what needs to go, then remove it after.
        std::vector<EntityHandle> to_remove;

        world.queryEntities<Transform, HomingTarget, MovementSpeed>(
            [&](EntityHandle handle, Transform& t, HomingTarget& homing, MovementSpeed& ms) {
                if (!world.isAlive(homing.target)) {
                    to_remove.push_back(handle);
                    return;
                }

                Transform* target_t = world.tryGet<Transform>(homing.target);
                if (target_t == nullptr) {
                    to_remove.push_back(handle);
                    return;
                }

                WorldSpacePos to_target =
                    WorldSpacePos{target_t->position.x, target_t->position.y} -
                    WorldSpacePos{t.position.x, t.position.y};
                float distance = to_target.length();

                constexpr float kHitRadius = 0.5f;
                if (distance < kHitRadius) {
                    to_remove.push_back(handle);
                    return;
                }

                WorldSpacePos direction = to_target / distance;
                WorldSpacePos movement = direction * ms.speed;
                t.position.x += movement.x;
                t.position.y += movement.y;
            }
        );

        for (const EntityHandle& handle : to_remove) {
            DespawnCommand cmd;
            cmd.server_handle = handle;
            DespawnPacket pkt;
            pkt.setData(cmd);
            net_adapter.sendPacket(&pkt, Channel::RELIABLECOMMANDS, {});

            world.removeEntity(handle);
        }
    }
};
