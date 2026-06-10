#pragma once

#include "Common/Coordinates/WorldSpacePos.hpp"

#include "Game/Match/Team.hpp"

struct SpawnPoint {
    WorldSpacePos point{};
    Team team{};
};