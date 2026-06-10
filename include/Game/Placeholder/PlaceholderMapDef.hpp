#pragma once

#include "Game/Match/MapDef.hpp"

namespace PlaceholderMapDef {
    inline MapDef getMap() {
        MapDef m;
        m.id = "summoners_rift";
        m.name = "Summoners Rift";
        m.n_teams = 2;
        m.max_players = 10;
        SpawnPoint sp;
        sp.point = {0,0};
        sp.team = {0};
        m.spawn_points.push_back(sp);
        m.model_path = "assets/Maps/SummonersRift.glb";
        return m;
    }
};