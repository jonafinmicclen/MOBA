#pragma once

#include <cstdint>
#include <variant>
#include <optional>
#include <vector>

#include "Networking/Core/NetCommon.hpp"   // PeerID, ChannelIDX etc.

// ---- Event payloads ----

struct NetConnectEvent {
    PeerID peer;
};

struct NetDisconnectEvent {
    PeerID peer;
};

// ---- Variant ----

using NetEvent = std::variant<
    NetConnectEvent,
    NetDisconnectEvent
>;