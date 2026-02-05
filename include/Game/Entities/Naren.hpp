#pragma once

#include "Game/Entities/Entitiy.hpp"

class Naren : Entity {
    public:
    ObjectState* getState() override { return &state; }

    private:
    ObjectState state;
};