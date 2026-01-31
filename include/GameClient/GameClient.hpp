#pragma once

#include <memory>

#include "Renderer/Renderer.hpp"
#include "Input/InputManager.hpp"
#include "Assets/ResourceManager.hpp"
#include "Game/Game.hpp"
#include "Assets/ModelData.hpp"
#include "Game/AssetState.hpp"
#include "Input/Listeners/ExitListener.hpp"
#include "Input/Listeners/ArrowKeyMoveListener.hpp"

#include "Debug/debug.hpp"

class GameClient {
private:
    std::unique_ptr<InputManager> inputManager; 
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Game> game;
    std::unique_ptr<ExitListener>  exitListener;
    std::unique_ptr<ArrowKeyMoveListener>  translationListener;
    std::unique_ptr<ResourceManager> resourceManager;

    AssetDatabase assetDatabase;

    std::vector<std::string> characterArgs;

    glm::vec3 translation = {0.0f, 0.0f, 0.0f};

    void initRenderer();

    int window_width = 1920;
    int window_height = 1080;

    bool running = true;
public:
    GameClient();
    void Run();
    void MoveCameraTowardsMouse();

};