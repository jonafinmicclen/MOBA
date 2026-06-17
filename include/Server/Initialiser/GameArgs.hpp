#pragma once

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "Common/Debug/debug.hpp"

#include <fstream>
#include <vector>
#include <stdexcept>
#include <string>

#include "Common/Player/AccountID.hpp"

#include "Game/Components/Match/Team.hpp"

using PlayerIdx = uint8_t;

struct Player {
    AccountID account;
    std::string character;
    Team team;
    PlayerIdx player_idx;
};

struct GameArgs {
    std::string map;
    std::vector<Player> players;
    std::unordered_map<std::string, std::string> player_account_map;

    GameArgs(const std::string path) {
        std::ifstream f(path);

        if (!f.is_open()) {
            throw std::runtime_error("Failed to open GameArgs file: " + path);
        }

        json game_args = json::parse(f);

        map = game_args.at("map").get<std::string>();

        PlayerIdx i = 0;

        for (const auto& p : game_args.at("players")) {
            Player player;

            player.account = p.at("account").get<std::string>();
            player.character = p.at("character").get<std::string>();

            player.team = Team{p.at("team").get<TeamIdx>()};
            player.player_idx = i++;

            players.push_back(player);
            DEBUG_LOG("Mapping " << player.account << " to " << player.character);
            player_account_map[player.account] = player.character;
        }
    }
};