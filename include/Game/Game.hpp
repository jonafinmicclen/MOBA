#pragma once

#include <memory>
#include <vector>

#include "Game/AssetState.hpp"
#include "Game/Entities/Entitiy.hpp"

class Game {
private:
    std::vector<Entity> entities;
public:
    Game();
    void update();

    std::vector<const AssetState*> getState();
};