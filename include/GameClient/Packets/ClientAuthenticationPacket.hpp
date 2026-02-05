#pragma once


#include "Networking/Packets/JSONPacket.hpp"


class ClientAuthenticationPacket : public AutoRegisterPacket<ClientAuthenticationPacket, PacketType::CLIENT_AUTH_PACKET>, public JSONPacket {
public:
    PacketType getType() const override { return PacketType::CLIENT_AUTH_PACKET; }
};
