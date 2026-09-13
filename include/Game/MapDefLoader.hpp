#pragma once

#include <filesystem>
#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

#include "Assets/WalkableMapLoader.hpp"
#include "Game/MapDef.hpp"

// Loads a MapDef from a map.json config (see assets/Maps/SRTemplate/map.json
// for the schema this expects). Replaces PlaceholderMapDef, which hardcoded
// all of this in C++ instead of reading it from data.
namespace MapDefLoader {

inline Rotation90 rotationFromDegrees(int deg) {
    switch (((deg % 360) + 360) % 360) {
        case 0:   return Rotation90::Deg0;
        case 90:  return Rotation90::Deg90;
        case 180: return Rotation90::Deg180;
        case 270: return Rotation90::Deg270;
        default:
            throw std::runtime_error("nav.transform.rot must be a multiple of 90, got " + std::to_string(deg));
    }
}

inline MapDef load(const std::string& map_json_path) {
    std::ifstream json_file(map_json_path);
    if (!json_file) {
        throw std::runtime_error("Failed to open map config: " + map_json_path);
    }

    nlohmann::json j;
    json_file >> j;

    // nav.map is relative to the map config's own directory, not the
    // process's working directory - so map folders stay self-contained.
    const std::filesystem::path map_dir = std::filesystem::path(map_json_path).parent_path();
    const std::filesystem::path walkable_path = map_dir / j.at("nav").at("map").get<std::string>();

    std::ifstream walkable_file(walkable_path, std::ios::binary);
    if (!walkable_file) {
        throw std::runtime_error("Failed to open walkable map file: " + walkable_path.string());
    }

    WalkableMap walkable_area = WalkableMapLoader::load(walkable_file);
    MapDef m(walkable_area);

    m.id = j.at("id").get<std::string>();
    m.name = j.at("display").at("name").get<std::string>();
    m.n_teams = static_cast<uint8_t>(j.at("rules").at("teams").size());
    m.max_players = j.at("rules").at("maxPlayers").get<uint8_t>();
    m.model_path = j.at("model").at("file").get<std::string>();

    m.map_boundary.min_corner = WorldSpacePos{
        Fixed(j.at("mapBoundary").at("min").at("x").get<float>()),
        Fixed(j.at("mapBoundary").at("min").at("y").get<float>())
    };
    m.map_boundary.max_corner = WorldSpacePos{
        Fixed(j.at("mapBoundary").at("max").at("x").get<float>()),
        Fixed(j.at("mapBoundary").at("max").at("y").get<float>())
    };

    for (const auto& sp : j.at("spawnPoints")) {
        SpawnPoint spawn;
        spawn.point = WorldSpacePos{
            Fixed(sp.at("x").get<float>()),
            Fixed(sp.at("y").get<float>())
        };
        spawn.team = Team{sp.at("team").get<TeamIdx>()};
        m.spawn_points.push_back(spawn);
    }

    const auto& transform = j.at("nav").at("transform");
    m.map_from_world = TransformGrid{
        .offset = Vec2{
            Fixed(transform.at("offset").at("x").get<float>()),
            Fixed(transform.at("offset").at("y").get<float>())
        },
        .scale = Vec2{
            Fixed(transform.at("scale").at("x").get<float>()),
            Fixed(transform.at("scale").at("y").get<float>())
        },
        .rotation = rotationFromDegrees(transform.at("rot").get<int>())
    };

    return m;
}

}
