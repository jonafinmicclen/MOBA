#include "Game/Game.hpp"

Game::Game() {

}

void Game::update() {
    for (auto& entity : entities) {
        entity->update();
    }
}