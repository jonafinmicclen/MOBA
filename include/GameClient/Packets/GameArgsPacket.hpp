#pragma once


#include "Networking/Packets/JSONPacket.hpp"


class GameArgsPacket : public AutoRegisterPacket<GameArgsPacket, PacketType::GAME_ARGS_PACKET>, public JSONPacket {
public:
    PacketType getType() const override { return PacketType::GAME_ARGS_PACKET; }
};
