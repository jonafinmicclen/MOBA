#pragma once

#include <cstdint>


enum class PacketFlag : uint8_t {
    GAMEPLAY = 0,
    MOVEMENT = 1,
    CHAT = 2
};