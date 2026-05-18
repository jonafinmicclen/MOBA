#pragma once


#include "Networking/Packets/JSONPacket.hpp"


class ClientAuthenticationPacket : public AutoRegisterPacket<ClientAuthenticationPacket, PacketType::CLIENT_AUTH_PACKET>, public JSONPacket {
public:
    PacketType getType() const override { return PacketType::CLIENT_AUTH_PACKET; }

    std::optional<const std::string> getHash() const {
        if (json_data.contains("hash") && json_data["hash"].is_string()) {
            return json_data["hash"].get<std::string>();
        }
        return std::nullopt;
    }
};
