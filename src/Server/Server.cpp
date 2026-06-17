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
    packet_distributor_.emplace();
    packet_manager_.emplace(*packet_distributor_, *net_server_);

    DEBUG_LOG("Loading config");
    loadConfig();
    DEBUG_LOG("Config loaded");

    client_auth_system_.emplace(*packet_distributor_, *net_adapter_, game_args_, client_state_.account_entity_map, map_, world_, client_state_.peer_id_account_hash_map);
    client_input_system_.emplace(*packet_distributor_, client_state_.peer_id_account_hash_map);
    net_server_->start();

    DEBUG_LOG("INIT DONE");
    
}


void Server::loadConfig() {
    // Just loading things into memory no world state
    // Load map
    ResourceManager::instance().loadAsset(game_args_.map);
    MapDef map = PlaceholderMapDef::getMap();
    map_ = map;

    DEBUG_LOG("Loading players");

    for (auto& player : game_args_.players) {
        ResourceManager::instance().loadAsset(player.character);
    }

}

void Server::simulate() {
    initialise();

    running = true;
    DEBUG_LOG("---RUNNING---");

    using clock = std::chrono::steady_clock;

    constexpr auto snapshot_interval = std::chrono::milliseconds(30);
    auto next_snapshot_time = clock::now();

    while (running) {
        packet_manager_->pump();

        client_input_system_->update(world_, client_state_.account_entity_map);
        path_following_system_.update(world_);

        const auto now = clock::now();

        if (now >= next_snapshot_time) {
            state_snapshot_system_.update(world_, *net_adapter_);

            next_snapshot_time = now + snapshot_interval;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}