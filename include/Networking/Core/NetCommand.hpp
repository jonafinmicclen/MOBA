// Networking/Core/NetCommand.hpp
#pragma once

#include "Networking/Core/NetCommon.hpp"

#include <string>
#include <variant>
#include <cstdint>
#include <optional>

struct ConnectCmd {
    std::string host;
    uint16_t port;
};

struct DisconnectCmd {
    std::optional<PeerID> id;
};

using NetCommand = std::variant<ConnectCmd, DisconnectCmd>;