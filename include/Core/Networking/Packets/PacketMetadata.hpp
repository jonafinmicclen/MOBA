#pragma once

#include "Core/Networking/Core/NetCommon.hpp"
#include "Core/Networking/Message/NetMessage.hpp"

#include <optional>

#include <vector>

struct PacketMetadata {
    PacketMetadata() = delete;
    PacketMetadata(NetMessage& msg) : ids(std::move(msg.peers)) {} 

    std::vector<PeerID> ids;
};