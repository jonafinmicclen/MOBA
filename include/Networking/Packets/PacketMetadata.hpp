#pragma once

#include "Networking/Core/NetCommon.hpp"
#include "Networking/Message/NetMessage.hpp"

#include <optional>

#include <vector>

struct PacketMetadata {
    PacketMetadata() = delete;
    PacketMetadata(NetMessage& msg) : ids(std::move(msg.peers)) {} 

    std::vector<PeerID> ids;
};