#pragma once

#include "Common/Coordinates/WorldSpacePos.hpp"

class IEntity {
private:
    int entityID;
    WorldSpacePos position;
public:
    virtual void update() {};
};