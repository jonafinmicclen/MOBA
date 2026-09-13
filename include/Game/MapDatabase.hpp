#pragma once

#include <string>
#include <unordered_map>

// Maps a game-args map name (e.g. "Summoners Rift") to the map.json config
// that describes it. Mirrors AssetDatabase's pattern (Game/CharacterMap/
// AssetDatabase.hpp), which does the same job for character meshes.
class MapDatabase {
public:
    static MapDatabase& instance() {
        static MapDatabase instance;
        return instance;
    }

    const std::string& Get(const std::string& name) const {
        return paths.at(name);
    }

    bool Exists(const std::string& name) const {
        return paths.find(name) != paths.end();
    }

    MapDatabase(const MapDatabase&) = delete;
    MapDatabase& operator=(const MapDatabase&) = delete;
    MapDatabase(MapDatabase&&) = delete;
    MapDatabase& operator=(MapDatabase&&) = delete;

private:
    MapDatabase()
        : paths {
            {"Summoners Rift", "assets/Maps/SRTemplate/map.json"}
        } {}

    std::unordered_map<std::string, std::string> paths;
};
