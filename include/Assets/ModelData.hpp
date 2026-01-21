#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <memory>
#include <cstdint>

struct Keyframe {
    float time;
    glm::vec3 translation;
    glm::quat rotation;
    glm::vec3 scale;
};

struct NodeAnimation {
    std::string nodeName;
    std::vector<Keyframe> keyframes;
};

struct AnimationData {
    std::string name;
    std::vector<NodeAnimation> channels;
};

struct Texture {
    GLuint id;
    int width;
    int height;
    int channels;
    std::vector<uint8_t> data;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 uv;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<std::shared_ptr<Texture>> textures;
};

struct Asset {
    std::unique_ptr<MeshData> mesh;
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<AnimationData> animations;
    std::string name;
};