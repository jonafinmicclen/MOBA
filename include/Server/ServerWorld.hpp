#pragma once

#include "Core/EntityComponentSystem/World.hpp"
#include "Core/EntityComponentSystem/Archetype.hpp"

#include "Game/Components/Transform.hpp"
#include "Game/Components/Navigation/Path.hpp"
#include "Game/Components/Match/Team.hpp"
#include "Game/Components/Match/Spawnpoint.hpp"
#include "Game/Components/Stats/MovementSpeed.hpp"
#include "Game/Components/Combat/HomingTarget.hpp"

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
            Archetype<Transform, Path, Team, SpawnPoint, MovementSpeed>
        >,

        ArchetypeSpec<
            ArchetypeId::Map,
            Archetype<Transform>
        >,

        ArchetypeSpec<
            ArchetypeId::ProjectileHoming,
            Archetype<Transform, HomingTarget, MovementSpeed>
        >
    >;
};

using ServerWorld = World<ServerWorldTraits>;
using ServerArchetypeId = ServerWorldTraits::ArchetypeId;