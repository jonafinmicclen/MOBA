#pragma once

#include "EntityComponentSystem/World.hpp"
#include "EntityComponentSystem/Archetype.hpp"

#include "Game/Components/Transform.hpp"
#include "Game/Components/Navigation/Path.hpp"
#include "Game/Components/Match/Team.hpp"
#include "Game/Components/Match/Spawnpoint.hpp"

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