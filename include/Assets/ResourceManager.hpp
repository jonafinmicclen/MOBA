#pragma once


#include <vector>
#include <string>
#include <memory>

#include <unordered_map>

#include "GLBLoader.hpp"
#include "Game/CharacterMap/CharacterDatabase.hpp"

#include "Assets/ModelData.hpp"

// Loads files...
// Assigns ID to different stuffs
// Simples

class ResourceManager {
    private:
    std::unordered_map<std::string, int> asset_indicies;
    std::vector<std::unique_ptr<Asset>> assets;

    int getAssetIndex(std::string asset_name);
    

    public:
    ResourceManager();
    void loadAsset(std::string asset_path, std::string asset_name);
    Asset* getAsset(std::string asset_name);
};