#include "Server/Server.hpp"

void Server::initialise() {
    DEBUG_LOG("INIT PHASE");

    NetConfig net_config(
        10,
        3,
        "localhost",
        8080
    );
    
    net_server_.emplace(net_config); 
    net_server_->start();
    net_adapter_.emplace(*net_server_);
    game_ = std::make_unique<Game>();

    DEBUG_LOG("LOADING CONFIG");
    loadConfig();
}


void Server::loadConfig() {

    GameArgs args("RuntimeData/game_args.json");

    ResourceManager::instance().loadAsset(args.map);
    game_->setMap(args.map);

    for (auto& player : args.players) {
        ResourceManager::instance().loadAsset(player.character);
    }

    game_args_ = std::move(args);
}


void Server::simulate() {
    initialise();
    running = true;
    DEBUG_LOG("---RUNNING---");

    while (running) {
        packet_manager_->pump();
    }
}