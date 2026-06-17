#pragma once

#include <external/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <memory>
#include <cstdint>
#include <optional>

struct MeshId{
    uint32_t id;
};

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
    GLuint id = 0;               // OpenGL texture ID (for GPU upload)
    int width = 0;
    int height = 0;
    int channels = 0;
    int uvIndex = 0;             // which UV set to sample
    std::vector<uint8_t> data;   // CPU-side image data
};


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv; // dynamic UV sets (TEXCOORD_0, TEXCOORD_1, etc.)
};


struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<int> faceTextureIndices;
};


struct Asset {
    MeshData mesh;
    std::vector<Texture> textures;
    std::vector<AnimationData> animations;
    MeshId mesh_id;
    std::string name;
};