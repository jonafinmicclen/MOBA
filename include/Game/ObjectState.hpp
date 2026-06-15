#pragma once

#include <string>
#include "EntityComponentSystem/Components/Transform.hpp"

struct ObjectState {
    std::string name;
    Transform transform;
};