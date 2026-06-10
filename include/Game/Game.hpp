#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "Debug/debug.hpp"

#include "Common/Memory/FIFOQueue.hpp"

#include "Game/ECS/World.hpp"

#include "Game/ObjectState.hpp"
#include "Game/Maps/Map.hpp"
#include "Game/Entities/Entitiy.hpp"

#include "Game/Maps/MapFactory.hpp"


class Game {
public:
    Game();
    void update();
    
    void setMap(const std::string name, std::optional<const uint32_t> mesh_id = std::nullopt);
    Map& getMap();
    World& getWorld();


private:
    World world_;
    std::shared_ptr<Map> map_;

};