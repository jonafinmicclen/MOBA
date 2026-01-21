#pragma once

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <iostream>

#include <unordered_map>

#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/Shader.hpp"
#include "Assets/ResourceManager.hpp"

class Renderer {
public:
    Renderer(ResourceManager* resManager, const int width, const int height);
    ~Renderer();

    void beginRender();
    void endRender();
    void testMesh(glm::vec3 translation);
    void uploadAssetMesh(Asset* asset);
    void drawMesh(const std::string& mesh_name, const glm::mat4& model);

private:
    struct GLMesh {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        size_t indexCount = 0;
        GLuint texture = 0;
    };

    glm::mat4 view;
    glm::mat4 proj;

    ResourceManager* resourceManager;
    SDL_Window* window;
    SDL_GLContext glContext;
    std::unordered_map<std::string, std::unique_ptr<GLMesh>> mesh_map;
    GLuint shaderProgram;

    int width, height;

};
