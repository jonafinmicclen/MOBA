#include "Game/Game.hpp"

Game::Game() {

}

void Game::setMap(std::string name) {
    map = std::move(MapFactory::instance().create(name));
    DEBUG_LOG(map->getName());
    addEntity(map);
}

Map& Game::getMap() {
    return *map;
}

void Game::update() {
}

std::vector<const ObjectState*> Game::getStates() {
    std::vector<const ObjectState*> states;
    states.reserve(entities.size());
    for (auto& entity : entities) {
        states.push_back(&entity->getState());
    }
    return states;
}

void Game::addEntity(std::shared_ptr<Entity> entity) {
    entities.push_back(entity);
}