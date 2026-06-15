#pragma once

#include "Common/Coordinates/WorldSpacePos.hpp"

#include "EntityComponentSystem/Components/Match/Team.hpp"

struct SpawnPoint {
    WorldSpacePos point{};
    Team team{0};
};