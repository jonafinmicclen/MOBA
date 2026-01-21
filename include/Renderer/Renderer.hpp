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

    void Render();

private:
    struct GLMesh {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        size_t indexCount = 0;
        GLuint texture = 0;
    };

    ResourceManager* resourceManager;
    SDL_Window* window;
    SDL_GLContext glContext;
    std::unordered_map<std::string, std::unique_ptr<GLMesh>> mesh_map;
    GLuint shaderProgram;

    int width, height;

    void drawMesh(const std::string mesh_name);
    void uploadAssetMesh(Asset* asset);
};
