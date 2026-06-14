#pragma once

#include "Game/Control/PlayerCommand.hpp"
#include "Game/Worlds/ServerWorld.hpp"
#include "Common/Memory/FIFOQueue.hpp"
#include "Common/Memory/BiMap.hpp"
#include "Debug/debug.hpp"
#include "Game/Transform.hpp"

using ClientInputQueue = FIFOQueue<ClientCommand>;

class ClientInputSystem{
public:
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
    ClientInputQueue& getQueue() {
        return queue_;
    }
private:
    ClientInputQueue queue_;
};