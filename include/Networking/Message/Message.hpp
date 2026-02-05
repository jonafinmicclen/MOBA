#pragma once

#include <memory>

#include "Networking/Packets/PacketBase.hpp"
#include "Networking/Message/MessageHeader.hpp"

struct Message {
    MessageHeader header;
    std::unique_ptr<PacketBase> packet;
};