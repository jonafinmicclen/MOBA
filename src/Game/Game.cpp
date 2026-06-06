#include "Game/Game.hpp"

Game::Game() {

}

void Game::setMap(const std::string name, std::optional<const uint32_t> mesh_id) {
    map_ = std::move(MapFactory::instance().create(name));
    auto& transform = map_->getState().transform;
    world_.add<ArchetypeId::Map>(transform, *mesh_id);
}

World& Game::getWorld() {
    return world_;
}

Map& Game::getMap() {
    return *map_;
}

void Game::update() {
}