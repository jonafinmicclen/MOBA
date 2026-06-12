#pragma once

#include "Game/ECS/World.hpp"
#include "Game/ECS/Archetype.hpp"

#include "Game/Transform.hpp"
#include "Game/Navigation/Path.hpp"
#include "Game/Match/Team.hpp"
#include "Game/Match/SpawnPoint.hpp"

#include <cstdint>
#include <tuple>

struct ServerWorldTraits {
    enum class ArchetypeId : uint8_t {
        Champion,
        Minion,
        ProjectileFree,
        ProjectileHoming,
        Turret,
        Map,
        None
    };

    using Archetypes = std::tuple<
        ArchetypeSpec<
            ArchetypeId::Champion,
            Archetype<Transform, Path, Team, SpawnPoint>
        >,

        ArchetypeSpec<
            ArchetypeId::Map,
            Archetype<Transform>
        >
    >;
};

using ServerWorld = World<ServerWorldTraits>;
using ServerArchetypeId = ServerWorldTraits::ArchetypeId;