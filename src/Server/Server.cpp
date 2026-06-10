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
    client_command_handler_.emplace(*packet_distributor_, client_command_system_.getQueue(), client_auth_manager_->getMap());
    net_server_->start();
    
}


void Server::loadConfig() {

    GameArgs args("RuntimeData/game_args.json");

    ResourceManager::instance().loadAsset(args.map);
    game_->setMap(args.map);
    MapDef map = PlaceholderMapDef::getMap();

    for (auto& player : args.players) {
        ResourceManager::instance().loadAsset(player.character);
        SpawnPoint player_spawn = map.spawn_points[player.player_idx];
        Path p;
        EntityHandle handle = game_->getWorld().add<ArchetypeId::Champion>(
            player_spawn.point, 0, p, player.team, player_spawn
        );
        account_entity_map_.insert(player.account, handle);

    }

    game_args_ = std::move(args);
    // By this point the loader will have returned MapDef and CharDef instances containing spawn points and etc
    
    // Initialise world
    //World world = game_->getWorld();
    //world.add<ArchetypeId::Champion>()
}


void Server::simulate() {
    initialise();
    running = true;
    DEBUG_LOG("---RUNNING---");

    while (running) {
        packet_manager_->pump();
    }
}