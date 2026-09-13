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
    SPAWN_DUMMY
};

struct ClientInput {
    ClientCommand command;
    WorldSpacePos mouse_pos;
    bool release;
    std::optional<AccountHash> account_hash = std::nullopt;
};
