#pragma once

#include "Game/Entities/Entitiy.hpp"

class Naren : public Entity {
    public:
    ObjectState& getState() override { return state; }
};