#pragma once

#include <cstdint>

enum class PacketType : uint8_t {
    Test_Packet = 0,
    JSON_Packet = 1,
    CLIENT_AUTH_PACKET = 2,
    GAME_ARGS_PACKET = 3,
    ClientCommandPacket = 4
};