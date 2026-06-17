#pragma once

#include "Core/EntityComponentSystem/World.hpp"
#include "Core/EntityComponentSystem/Archetype.hpp"

#include "Game/Components/Transform.hpp"
#include "Game/Components/Navigation/Path.hpp"
#include "Game/Components/Match/Team.hpp"
#include "Game/Components/Match/Spawnpoint.hpp"

#include "Assets/ModelData.hpp"

#include <cstdint>
#include <tuple>

struct ClientWorldTraits {
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
            Archetype<Transform, MeshId, EntityHandle>
        >,

        ArchetypeSpec<
            ArchetypeId::Map,
            Archetype<Transform, MeshId>
        >
    >;
};

using ClientWorld = World<ClientWorldTraits>;
using ClientArchetypeId = ClientWorldTraits::ArchetypeId;