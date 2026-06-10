#pragma once

#include "Game/Control/PlayerCommand.hpp"
#include "Game/ECS/World.hpp"
#include "Common/Memory/FIFOQueue.hpp"
#include "Common/Memory/BiMap.hpp"
#include "Debug/debug.hpp"

using ClientCommandQueue = FIFOQueue<ClientCommand>;

class ClientCommandSystem{
public:
    void update(World& world, BiMap<AccountHash, EntityHandle> acc_entity_map) {
        while (auto c = queue_.pop()) {
            auto& hash = c->account_hash;
            if(!hash) {
                DEBUG_LOG("hash not present");
                continue;
            }
            EntityHandle* handle = acc_entity_map.findByA(*hash);
            if (handle == nullptr) {
                DEBUG_LOG("Command recieved from non-assigned");
            }
            Transform* t = world.tryGet<Transform>(*handle);
            if (t == nullptr) {
                DEBUG_LOG("No transform found when applying command");
            }
            t->position.x = c->mouse_pos.x;
            t->position.y = c->mouse_pos.y;
        }       
    }
    ClientCommandQueue& getQueue() {
        return queue_;
    }
private:
    ClientCommandQueue queue_;
};