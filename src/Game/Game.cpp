#include "Game/Game.hpp"

<<<<<<< HEAD
Game::Game() {

}

void Game::setMap(std::string name) {
    map_ = std::move(MapFactory::instance().create(name));
    addEntity(map_);
}

Map& Game::getMap() {
    return *map_;
}

void Game::update() {
}

std::vector<const ObjectState*> Game::getStates() {
    std::vector<const ObjectState*> states;
    states.reserve(entities_.size());
    for (auto& entity : entities_) {
        states.push_back(&entity->getState());
    }
    return states;
}

void Game::addEntity(std::shared_ptr<Entity> entity) {
    entities_.push_back(entity);
}
=======
>>>>>>> origin/main
