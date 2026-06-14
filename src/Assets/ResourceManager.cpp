#include "Assets/ResourceManager.hpp"
#include "Renderer/Renderer.hpp"

ResourceManager::ResourceManager() {
}

AssetId ResourceManager::getAssetId(std::string asset_name) {
    return asset_indicies.at(asset_name);
}

void ResourceManager::loadAsset(std::string asset_path, std::string asset_name) {
    if (asset_indicies.find(asset_name) != asset_indicies.end()) {
        DEBUG_LOG(asset_name << " already loaded");
        return;
    }
    // Report asset being loaded
    DEBUG_LOG(asset_path);

    // Load GLB
    auto loader = GLBLoader(asset_path.c_str());
    auto& loaded_asset = *loader.asset_data;

    // Upload to gpu
    DEBUG_LOG("Uploading " << asset_name << " to GPU");
    MeshId mid = std::numeric_limits<MeshId>::max();
    if (renderer_ != nullptr) {
        mid = renderer_->uploadAssetMesh(loaded_asset);
    } else {
        DEBUG_LOG("No renderer, skipping GPU upload");
    }

    // Configure asset detail
    loaded_asset.name = asset_name;
    loaded_asset.mesh_id = mid;

    // Store asset in manager
    assets.push_back(std::move(loaded_asset));
    asset_indicies[asset_name] = static_cast<AssetId>(assets.size()-1);
    return;
} 

void ResourceManager::loadAsset(std::string name) {
    auto path = database.Get(name).asset_path;
    loadAsset(path, name);
}

Asset* ResourceManager::getAsset(std::string asset_name) {
    AssetId idx = getAssetId(asset_name);
    return getAsset(idx);
}

Asset* ResourceManager::getAsset(AssetId asset_id) {
    assert((asset_id < assets.size()) && "Attempted to get asset by an id that did not exist");
    return &assets[asset_id];
}