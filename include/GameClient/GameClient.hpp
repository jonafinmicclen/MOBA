#pragma once

#include <memory>

#include "Renderer/Renderer.hpp"
#include "Input/InputManager.hpp"
#include "Assets/ResourceManager.hpp"
#include "Game/Game.hpp"
#include "Assets/ModelData.hpp"

#include "Input/Listeners/ExitListener.hpp"

class GameClient {
private:
    std::unique_ptr<InputManager> inputManager; 
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Game> game;
    std::unique_ptr<ExitListener>  exitListener;
    std::unique_ptr<ResourceManager> resourceManager;

    CharacterDatabase characterDatabase;

    std::vector<std::string> characterArgs;

    bool running = true;
public:
    GameClient();
    void Run();

};