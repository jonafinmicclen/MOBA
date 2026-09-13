#include "Assets/ResourceManager.hpp"
#include "Client/Renderer/Renderer.hpp"
#include "Game/CharacterMap/CharacterDefLoader.hpp"

namespace {
    bool hasExtension(const std::string& path, const std::string& ext) {
        return path.size() >= ext.size() &&
            path.compare(path.size() - ext.size(), ext.size(), ext) == 0;
    }
}

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

    // A character asset points at a character.json rather than a .glb
    // directly - resolve the real mesh path and rest-pose correction
    // through it first.
    glm::mat4 rest_pose_transform{1.0f};
    std::string glb_path = asset_path;
    if (hasExtension(asset_path, ".json")) {
        CharacterDef def = CharacterDefLoader::load(asset_path);
        glb_path = def.glb_path;
        rest_pose_transform = def.rest_pose_transform;
    }

    // Report asset being loaded
    DEBUG_LOG(glb_path);

    // Load GLB
    auto loader = GLBLoader(glb_path.c_str());
    auto& loaded_asset = *loader.asset_data;

    // Bake the rest-pose correction into the mesh once here, so every
    // downstream consumer (gameplay rotation, animation, rendering) can
    // treat the mesh as already standing correctly at identity transform -
    // matches PathFollowingSystem::faceDirection's assumption.
    glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(rest_pose_transform)));
    for (auto& vertex : loaded_asset.mesh.vertices) {
        vertex.position = glm::vec3(rest_pose_transform * glm::vec4(vertex.position, 1.0f));
        vertex.normal = glm::normalize(normal_matrix * vertex.normal);
    }

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