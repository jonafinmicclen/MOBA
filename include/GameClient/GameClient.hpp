#pragma once


#include "Renderer/Renderer.hpp"
#include "Input/InputManager.hpp"
#include "Game/Game.hpp"

class GameClient {
private:
    InputManager inputManager; 
    Renderer renderer;
    Game game;
public:
    GameClient();
    void update();
};