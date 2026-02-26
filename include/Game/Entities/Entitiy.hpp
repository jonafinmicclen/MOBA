#pragma once

#include <string>

#include "Game/ObjectState.hpp"

class Entity {
    public:
    virtual ObjectState& getState() = 0;

    protected:
    ObjectState state;
};