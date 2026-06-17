#pragma once

#include "Server/Initialiser/GameArgs.hpp"
#include "Core/Networking/Packets/JSONPacket.hpp"


class GameArgsPacket : public AutoRegisterPacket<GameArgsPacket, PacketType::GAME_ARGS_PACKET>, public JSONPacket {
public:
    static GameArgsPacket fromArgs(GameArgs& args) {
        GameArgsPacket packet;
        auto& data = packet.get_json();
        
        std::vector<std::string> all_assets {args.map};
        for (auto& player : args.players) {
            all_assets.push_back(player.character);
        }
        
        data["map"] = args.map;
        data["all_assets"] = all_assets;

        return packet;
    }
    PacketType getType() const override { return PacketType::GAME_ARGS_PACKET; }
};
