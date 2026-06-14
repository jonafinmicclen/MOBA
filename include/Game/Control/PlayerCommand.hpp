#pragma once

#include <cstdint>
#include "Common/Coordinates/WorldSpacePos.hpp"
#include <optional>
#include "Authentication/Auth.hpp"

enum class ClientButton : uint8_t {
    LEFT_CLICK,
    RIGHT_CLICK,
    Q,
    W,
    E,
    R,
    D,
    F,
    ITEMSLOT1,
    ITEMSLOT2,
    ITEMSLOT3,
    ITEMSLOT4,
    ITEMSLOT5,
    ITEMSLOT6
};

struct ClientCommand {
    ClientButton btn;
    WorldSpacePos mouse_pos;
    bool release;
    std::optional<AccountHash> account_hash = std::nullopt;
};
