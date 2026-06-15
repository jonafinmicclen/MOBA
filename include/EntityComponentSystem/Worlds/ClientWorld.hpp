#pragma once

#include "EntityComponentSystem/Core/World.hpp"
#include "EntityComponentSystem/Core/Archetype.hpp"

#include "EntityComponentSystem/Components/Transform.hpp"
#include "EntityComponentSystem/Components/Navigation/Path.hpp"
#include "EntityComponentSystem/Components/Match/Team.hpp"
#include "EntityComponentSystem/Components/Match/Spawnpoint.hpp"

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