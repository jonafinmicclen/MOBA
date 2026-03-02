#pragma once

#include "Server/GameArgs.hpp"
#include "Networking/Packets/JSONPacket.hpp"


class GameArgsPacket : public AutoRegisterPacket<GameArgsPacket, PacketType::GAME_ARGS_PACKET>, public JSONPacket {
public:
    explicit GameArgsPacket(GameArgs& args) {
        
        std::vector<std::string> all_assets;
        for (auto& player : args.players) {
            all_assets.push_back(player.character);
        }
        all_assets.push_back(args.map);

        json_data["map"] = args.map;
        json_data["all_assets"] = all_assets;
    }
    PacketType getType() const override { return PacketType::GAME_ARGS_PACKET; }
};
