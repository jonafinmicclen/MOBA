#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <fstream>
#include <vector>

#include "Common/Player/AccountID.hpp"
#include "Game/Entities/Characters/CharacterID.hpp"

#include "Server/AccountCharacterBiMap.hpp"

struct Player {
    AccountID account;
    CharacterID character;
};

struct GameArgs {
    std::string map;
    std::vector<Player> players;
    AccountCharacterBiMap player_account_map;

    GameArgs(const std::string path) {
        std::ifstream f("RuntimeData/game_args.json");
        json game_args = json::parse(f);
        map = game_args["map"];
        for (auto& [account, character] : game_args["players"].items()) {
            Player player;
            player.account = account;
            player.character = character;
            players.push_back(player);
            player_account_map.bind(account, character);
        }    
    }
};