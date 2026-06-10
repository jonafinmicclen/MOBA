#pragma once

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <iostream>

#include <unordered_map>

#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/Shader.hpp"
#include "Assets/ResourceManager.hpp"
#include "Assets/ModelData.hpp"
#include <limits>

#include "GameClient/Camera/Camera.hpp"
#include <glm/gtx/string_cast.hpp>



class Renderer {
public:
    Renderer(const int width, const int height);
    ~Renderer();

    void beginRender();
    void endRender();
    void testMesh(glm::vec3 translation);
    MeshId uploadAssetMesh(Asset* asset);
    void drawMesh(const MeshId mesh_id, const glm::mat4& model);

    void setCamera(Camera* cam) {camera = cam; view=cam->getView(); proj=camera->getProjection((float)width/(float)height); }

private:
    struct GLMesh {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        size_t indexCount = 0;

        std::vector<GLuint> textures;        // All textures for this mesh
        std::unordered_map<int, std::vector<uint32_t>> textureToFaceIndices;

    };

    Camera* camera;

    ResourceManager& resourceManager = ResourceManager::instance();
    SDL_Window* window;
    SDL_GLContext glContext;
    GLuint shaderProgram;

    glm::mat4 view;
    glm::mat4 proj;

    GLint uModel = -1;
    GLint uView  = -1;
    GLint uProj  = -1;
    GLint uTex   = -1;

    int width, height;

    MeshId next_mesh_id_ = 0;
    std::vector<std::unique_ptr<GLMesh>> meshes;
};
