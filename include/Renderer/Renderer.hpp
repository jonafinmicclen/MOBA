#pragma once

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/Shader.hpp"
#include "Assets/ResourceManager.hpp"

class Renderer {
public:
    Renderer(ResourceManager* resManager);
    ~Renderer();

    void Render();

private:
    ResourceManager* resourceManager;
    SDL_Window* window;
    SDL_GLContext glContext;

    GLuint shaderProgram;

    struct GLMesh {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        size_t indexCount = 0;
        GLuint texture = 0;
    };

    GLMesh uploadAssetMesh(Asset* asset);
};
