#pragma once

#include <string>

#include "Game/ObjectState.hpp"

class Entity {
    public:
    ObjectState& getState() {return state;}

    protected:
    ObjectState state;
};