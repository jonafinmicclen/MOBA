#pragma once

#include "Networking/PacketFlag.hpp"
#include <string>
#include <optional>

struct MessageHeader {
    PacketFlag flag = PacketFlag::GAMEPLAY;
    std::optional<uint8_t> id;
    std::string address = "";
};