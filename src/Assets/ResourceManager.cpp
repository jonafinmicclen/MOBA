#include "Assets/ResourceManager.hpp"

ResourceManager::ResourceManager() {
}

int ResourceManager::getAssetIndex(std::string asset_name) {
    return asset_indicies.at(asset_name);
}

void ResourceManager::loadAsset(std::string asset_path, std::string asset_name) {

    std::cout<<"[ResourceManager] Loading " << asset_path<<std::endl;
    auto loader = GLBLoader(asset_path.c_str());
    loader.asset_data->name = asset_name;
    assets.push_back(std::move(loader.asset_data));
    asset_indicies[asset_name] = (int)assets.size()-1;
    return;
} 

void ResourceManager::loadAsset(std::string name) {
    auto path = database.Get(name).asset_path;
    loadAsset(path, name);
}

Asset* ResourceManager::getAsset(std::string asset_name) {
    int idx = getAssetIndex(asset_name);
    return assets[idx].get();
}