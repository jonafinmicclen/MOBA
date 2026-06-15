#pragma once

#include "EntityComponentSystem/Worlds/ServerWorld.hpp"
#include "Game/Packets/EntityStatePacket.hpp"
#include "Adapter/NetAdapter.hpp"

class StateSnapshotSystem{
public:
    void update(ServerWorld& world, NetAdapter& net_adapter) {
        world.queryEntities<Transform>(
            [&net_adapter](EntityHandle handle, Transform transform) {
                EntityStateUpdate data;
                data.affected_fields = EntityFieldIdentifier::Transform;
                data.handle = handle;
                data.new_transform = transform;
                EntityStatePacket pkt;
                pkt.setData(data);
                net_adapter.sendPacket(&pkt, Channel::STATEUPDATES, {});
            }
        );
    }
};