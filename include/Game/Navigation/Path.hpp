#pragma once

#include <array>
#include <glm/glm.hpp>

constexpr size_t MAX_WAYPOINTS = 5;

struct Path {
    std::array<glm::vec2, MAX_WAYPOINTS> waypoints;
};
