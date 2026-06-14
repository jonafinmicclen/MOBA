#pragma once


#include <vector>
#include <string>
#include <memory>
#include <cstdint>

#include <unordered_map>

#include "GLBLoader.hpp"
#include "Debug/debug.hpp"
#include "Game/CharacterMap/AssetDatabase.hpp"
#include "Assets/ModelData.hpp"

// Loads files...
// Assigns ID to different stuffs
// Simples

class Renderer;

using AssetId = uint32_t;

class ResourceManager {
    public:
    static ResourceManager& instance() {
        static ResourceManager instance;
        return instance;
    }

    void init(Renderer* r) {renderer_ = r;}
    
    void loadAsset(std::string asset_path, std::string asset_name);
    void loadAsset(std::string asset_name);
    Asset* getAsset(std::string asset_name);
    Asset* getAsset(AssetId asset_id);

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    private:
    ResourceManager();
    AssetId getAssetId(std::string asset_name);

    std::unordered_map<std::string, AssetId> asset_indicies;
    std::vector<Asset> assets;
    AssetDatabase& database = AssetDatabase::instance();

    Renderer* renderer_ = nullptr;

};