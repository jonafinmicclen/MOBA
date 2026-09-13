#pragma once

#include <cstdint>
#include "Common/Coordinates/WorldSpacePos.hpp"
#include <optional>
#include "Authentication/Auth.hpp"

// Semantic action sent over the network - what the server actually acts on.
// Distinct from ClientButton (include/Client/Input/ClientButton.hpp), the
// physical key/mouse button that triggers it - that mapping is
// client-side only and configurable (see ClientConfig's "keybindings").
enum class ClientCommand : uint8_t {
    MOVE,
    SPAWN_DUMMY,
    Q_ABILITY
};

struct ClientInput {
    ClientCommand command;
    WorldSpacePos mouse_pos;
    bool release;
    // Generic per-command radius, in world units - e.g. Q_ABILITY's target
    // search radius around mouse_pos (ClientConfig's qAbilityTargetRadius,
    // "mouse proximity sensitivity"). Sent unconditionally like mouse_pos;
    // commands that don't need it just ignore it.
    Fixed radius{};
    std::optional<AccountHash> account_hash = std::nullopt;
};
