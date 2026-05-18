#pragma once

#include <unordered_map>
#include <string>

#include "Game/CharacterMap/AssetDefinition.hpp"

class AssetDatabase {
    public:
    static AssetDatabase& instance() {
        static AssetDatabase instance;  // created once
        return instance;
    }

    const AssetDefinition& Get(const std::string& id) const {
        return defs.at(id);
    }

    bool Exists(const std::string& id) const {
        return defs.find(id) != defs.end();
    }

    AssetDatabase(const AssetDatabase&) = delete;
    AssetDatabase& operator=(const AssetDatabase&) = delete;
    AssetDatabase(AssetDatabase&&) = delete;
    AssetDatabase& operator=(AssetDatabase&&) = delete;

    private:
    AssetDatabase()
        :   defs {
            {"Naren", {"Naren", "assets/Characters/Naren/Naren.glb"}},
            {"Map", {"Map", "assets/Maps/Map.glb"}},
            {"Summoners Rift", {"Summoners Rift", "assets/Maps/SummonersRift.glb"}}
        }   {}
        
    std::unordered_map<std::string, AssetDefinition> defs;

};