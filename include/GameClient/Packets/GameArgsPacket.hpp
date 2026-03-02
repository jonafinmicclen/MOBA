#pragma once

#include "Server/GameArgs.hpp"
#include "Networking/Packets/JSONPacket.hpp"


class GameArgsPacket : public AutoRegisterPacket<GameArgsPacket, PacketType::GAME_ARGS_PACKET>, public JSONPacket {
public:
    static GameArgsPacket fromArgs(GameArgs& args) {
        GameArgsPacket packet;
        auto& data = packet.get_json();
        
        std::vector<std::string> all_assets;
        for (auto& player : args.players) {
            all_assets.push_back(player.character);
        }
        all_assets.push_back(args.map);
        
        data["map"] = args.map;
        data["all_assets"] = all_assets;

        return packet;
    }
    PacketType getType() const override { return PacketType::GAME_ARGS_PACKET; }
};
