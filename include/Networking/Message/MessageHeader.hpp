#pragma once

#include "Networking/PacketFlag.hpp"
#include <string>

struct MessageHeader {
    PacketFlag flag = PacketFlag::GAMEPLAY;
    std::string address = "";
};