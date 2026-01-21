#pragma once

#include <string>
#include "Common/Coordinates/WorldSpacePos.hpp"
#include <glm/gtc/matrix_transform.hpp>

struct AssetState {
    std::string name;
    glm::vec3 position;
    glm::vec3 scale;
    float rot_angle;
    glm::vec3 rot_axis;
};