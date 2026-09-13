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

        #ifdef DEBUG
        assert(Exists(id) && "Asset definition did not exist, program will now crash XD");
        #endif

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
            // Naren points at a character.json (see CharacterDefLoader) -
            // ResourceManager::loadAsset detects the .json extension and
            // resolves the actual .glb plus rest-pose correction through
            // it, rather than loading this path directly as a mesh.
            {"Naren", {"Naren", "assets/Characters/Naren/character.json"}},
            {"Map", {"Map", "assets/Maps/Map.glb"}},
            {"Summoners Rift", {"Summoners Rift", "assets/Maps/SummonersRift.glb"}}
        }   {}
        
    std::unordered_map<std::string, AssetDefinition> defs;

};