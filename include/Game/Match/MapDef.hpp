#pragma once

#include <vector>
#include <string>

#include "Game/Match/SpawnPoint.hpp"


struct MapDef{
    std::string id{};
    std::string name{};
    uint8_t n_teams{};
    uint8_t max_players{};
    std::vector<SpawnPoint> spawn_points;
    std::string model_path {};
};