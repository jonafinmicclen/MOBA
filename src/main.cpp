#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <box2d/box2d.h>
#include <enet/enet.h>
#include <iostream>
    
int main(int argc, char* argv[]) {
    // --- SDL2 + OpenGL Window ---
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("MyMOBA", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return 1;
    }

    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);

    // --- Box2D World ---
    b2Vec2 gravity(0.0f, -9.8f);
    b2World world(gravity);

    // --- ENet Initialization ---
    if (enet_initialize() != 0) {
        std::cerr << "Failed to initialize ENet" << std::endl;
        return 1;
    }
    atexit(enet_deinitialize);

    // Simple ENet server (example)
    ENetAddress address;
    ENetHost* server;
    address.host = ENET_HOST_ANY;
    address.port = 1234;
    server = enet_host_create(&address, 32, 2, 0, 0);
    if (!server) {
        std::cerr << "Failed to create ENet server" << std::endl;
        return 1;
    }

    std::cout << "Server running on port 1234..." << std::endl;

    // --- Main Loop ---
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
        }

        // Clear screen
        glClear(GL_COLOR_BUFFER_BIT);

        // TODO: Add game rendering here

        SDL_GL_SwapWindow(window);
    }

    enet_host_destroy(server);
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
