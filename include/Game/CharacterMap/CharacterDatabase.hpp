#pragma once

#include <unordered_map>

#include "Game/CharacterMap/CharacterDefinition.hpp"

class CharacterDatabase {
    std::unordered_map<std::string, CharacterDefinition> defs;

    public:
    CharacterDatabase(){
        defs = {
            {"Naren", {"Naren", "assets/Characters/Naren/Naren.glb"}}
        };
    }

    const CharacterDefinition& Get(const std::string& id) const {
        return defs.at(id);
    }

    bool Exists(const std::string& id) const {
        return defs.find(id) != defs.end();
    }
};