#include "GameClient/GameClient.hpp"


GameClient::GameClient() {
    inputManager = InputManager();
    renderer = Renderer();
    game = Game();
}

void GameClient::update() {
    game.update();
}