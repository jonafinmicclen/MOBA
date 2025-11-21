#pragma once

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <iostream>

class Renderer {
    public:
    Renderer();
    ~Renderer();

    private:
    SDL_GLContext glContext;
    SDL_Window* window;
};