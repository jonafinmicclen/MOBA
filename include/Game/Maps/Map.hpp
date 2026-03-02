#pragma once

#include "Game/Entities/Entitiy.hpp"

#include <string>

class Map : public Entity {
    public:
    virtual std::string getName() = 0;
    private:
};