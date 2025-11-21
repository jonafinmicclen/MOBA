#pragma once

#include "Game/Entities/IEntity.hpp"
#include <memory>
#include <vector>

class Game {
private:
    std::vector<std::unique_ptr<IEntity>> entities;
public:
    Game();
    void update();
    
};