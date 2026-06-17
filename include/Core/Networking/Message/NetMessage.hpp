#pragma once

#include "Core/Networking/Core/NetCommon.hpp"
#include <enet/enet.h>

#include <vector>

/**
 * @brief Represents incoming or outgoing messages
 * 
 * When peer vector is empty this will be sent to all
 */

struct NetMessage
{
    ChannelIDX channel_idx;
    enet_uint32 flags;
    std::vector<PeerID> peers;
    std::vector<uint8_t> bytes;

    NetMessage() = delete;
    NetMessage(ChannelIDX ch,
               enet_uint32 f,
               std::vector<uint8_t> b,
               std::vector<PeerID> p = {})
        : channel_idx(ch),
          flags(f),
          peers(std::move(p)),
          bytes(std::move(b))
    {}
};