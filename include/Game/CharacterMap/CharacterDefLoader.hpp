#pragma once

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include <nlohmann/json.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

// Loads a character's config (see assets/Characters/Naren/character.json for
// the schema) - name/description, where its .glb actually is, and the rest
// pose correction needed so the model stands upright facing the direction
// PathFollowingSystem::faceDirection assumes (world +X at identity
// rotation). That correction gets baked directly into the mesh's vertices
// once at load time (see ResourceManager::loadAsset), not reapplied per
// frame, so nothing downstream (gameplay rotation, animation, etc.) needs
// to know about it.
struct CharacterDef {
    std::string name;
    std::string description;
    std::string glb_path;
    glm::mat4 rest_pose_transform{1.0f};
};

namespace CharacterDefLoader {

inline CharacterDef load(const std::string& character_json_path) {
    std::ifstream file(character_json_path);
    if (!file) {
        throw std::runtime_error("Failed to open character config: " + character_json_path);
    }

    nlohmann::json j;
    file >> j;

    const std::filesystem::path dir = std::filesystem::path(character_json_path).parent_path();

    CharacterDef def;
    def.name = j.at("name").get<std::string>();
    def.description = j.value("description", "");
    def.glb_path = (dir / j.at("model").at("file").get<std::string>()).string();

    glm::vec3 offset(0.0f);
    glm::vec3 rotation_degrees(0.0f);
    glm::vec3 scale(1.0f);

    if (j.contains("restPoseTransform")) {
        const auto& rp = j.at("restPoseTransform");
        if (rp.contains("offset")) {
            const auto& o = rp.at("offset");
            offset = { o.value("x", 0.0f), o.value("y", 0.0f), o.value("z", 0.0f) };
        }
        if (rp.contains("rotationDegrees")) {
            const auto& r = rp.at("rotationDegrees");
            rotation_degrees = { r.value("x", 0.0f), r.value("y", 0.0f), r.value("z", 0.0f) };
        }
        if (rp.contains("scale")) {
            const auto& s = rp.at("scale");
            scale = { s.value("x", 1.0f), s.value("y", 1.0f), s.value("z", 1.0f) };
        }
    }

    glm::mat4 m(1.0f);
    m = glm::translate(m, offset);
    m = m * glm::mat4_cast(glm::quat(glm::radians(rotation_degrees)));
    m = glm::scale(m, scale);
    def.rest_pose_transform = m;

    return def;
}

}
