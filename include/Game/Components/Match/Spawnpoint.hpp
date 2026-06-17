#pragma once

#include "Common/Coordinates/WorldSpacePos.hpp"

#include "Game/Components/Match/Team.hpp"

struct SpawnPoint {
    WorldSpacePos point{};
    Team team{0};
};