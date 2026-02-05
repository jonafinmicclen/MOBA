#pragma once


#include <vector>
#include <string>
#include <memory>

#include <unordered_map>

#include "GLBLoader.hpp"
#include "Game/CharacterMap/AssetDatabase.hpp"

#include "Assets/ModelData.hpp"

// Loads files...
// Assigns ID to different stuffs
// Simples

class ResourceManager {
    public:
    static ResourceManager& instance() {
        static ResourceManager instance;
        return instance;
    }
    
    void loadAsset(std::string asset_path, std::string asset_name);
    void loadAsset(std::string asset_name);
    Asset* getAsset(std::string asset_name);

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    private:
    ResourceManager();
    int getAssetIndex(std::string asset_name);

    std::unordered_map<std::string, int> asset_indicies;
    std::vector<std::unique_ptr<Asset>> assets;
    AssetDatabase& database = AssetDatabase::instance();

};