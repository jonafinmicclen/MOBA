#pragma once

#include <string>

#include "Game/AssetState.hpp"

class Entity {
    public:
    virtual AssetState* getState();
};