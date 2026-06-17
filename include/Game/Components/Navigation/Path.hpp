#pragma once

#include <array>
#include "Common/Coordinates/WorldSpacePos.hpp"

constexpr size_t MAX_WAYPOINTS = 35;

using Waypoint = WorldSpacePos;
using Waypoints = std::array<Waypoint, MAX_WAYPOINTS>;

struct Path {
    Waypoints waypoints;
    size_t target_waypoint;
    size_t num_waypoints;
    bool active = false;
};
 