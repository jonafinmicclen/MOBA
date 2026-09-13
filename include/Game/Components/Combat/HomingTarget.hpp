#pragma once

#include "Core/EntityComponentSystem/World.hpp"

// The entity a homing projectile is chasing. See ProjectileHomingSystem,
// which despawns the projectile if this entity dies/disappears.
struct HomingTarget {
    EntityHandle target;
};
