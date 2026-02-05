#pragma once

#include <string>

#include "Game/ObjectState.hpp"

class Entity {
    public:
    virtual ObjectState* getState();
};