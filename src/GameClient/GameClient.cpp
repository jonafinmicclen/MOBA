#include "GameClient/GameClient.hpp"


GameClient::GameClient() {
    inputManager = InputManager();
    renderer = Renderer();
    game = Game();
}