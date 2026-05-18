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
    net_adapter_.emplace(*net_server_);
    game_ = std::make_unique<Game>();
    packet_distributor_.emplace();
    packet_manager_.emplace(*packet_distributor_, *net_server_);

    loadConfig();

    client_auth_manager_.emplace(*packet_distributor_, *net_adapter_, *game_args_);
    net_server_->start();
    
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