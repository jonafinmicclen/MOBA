#pragma once

#include <memory>
#include <vector>

#include "Debug/debug.hpp"

#include "Game/ObjectState.hpp"
#include "Game/Maps/Map.hpp"
#include "Game/Entities/Entitiy.hpp"

#include "Game/Maps/MapFactory.hpp"

class Game {
public:
    Game();
    void update();
    void setMap(const std::string name);
    Map& getMap();

    std::vector<const ObjectState*> getStates();
private:
    std::vector<Entity> entities;

    std::unique_ptr<Map> map;
};