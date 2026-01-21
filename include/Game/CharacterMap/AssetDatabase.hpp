#pragma once

#include <unordered_map>

#include "Game/CharacterMap/AssetDefinition.hpp"

class AssetDatabase {
    std::unordered_map<std::string, AssetDefinition> defs;

    public:
    AssetDatabase(){
        defs = {
            {"Naren", {"Naren", "assets/Characters/Naren/Naren.glb"}},
            {"Map", {"Map", "assets/Maps/Map.glb"}}
        };
    }

    const AssetDefinition& Get(const std::string& id) const {
        return defs.at(id);
    }

    bool Exists(const std::string& id) const {
        return defs.find(id) != defs.end();
    }
};