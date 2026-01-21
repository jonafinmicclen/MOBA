#include "Game/Game.hpp"

Game::Game() {

}

void Game::update() {
}

std::vector<const AssetState*> Game::getState() {
    std::vector<const AssetState*> states;
    states.reserve(entities.size());
    for (auto& entity : entities) {
        states.push_back(entity.getState());
    }
    return states;
}