#pragma once

#include <cstdint>

enum class PacketType : uint8_t {
    CastAbility = 0,
    MoveCharacter = 1
};