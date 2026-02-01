#pragma once

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <iostream>

#include <unordered_map>

#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/Shader.hpp"
#include "Assets/ResourceManager.hpp"

#include "GameClient/Camera/Camera.hpp"
#include <glm/gtx/string_cast.hpp>

class Renderer {
public:
    Renderer(ResourceManager* resManager, const int width, const int height);
    ~Renderer();

    void beginRender();
    void endRender();
    void testMesh(glm::vec3 translation);
    void uploadAssetMesh(Asset* asset);
    void drawMesh(const std::string& mesh_name, const glm::mat4& model);

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

    ResourceManager* resourceManager;
    SDL_Window* window;
    SDL_GLContext glContext;
    std::unordered_map<std::string, std::unique_ptr<GLMesh>> mesh_map;
    GLuint shaderProgram;

    glm::mat4 view;
    glm::mat4 proj;

    int width, height;

};
