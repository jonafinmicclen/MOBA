#pragma once

#include "Game/Entities/Entitiy.hpp"

class Naren : Entity {
    public:
    AssetState* getState() override { return &state; }

    private:
    AssetState state;
};