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

    client_auth_manager_.emplace(*packet_distributor_, *net_adapter_, game_args_, account_entity_map_, map_, world_);
    client_command_handler_.emplace(*packet_distributor_, client_command_system_.getQueue(), client_auth_manager_->getMap());
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

void Server::initialiseWorld() {
    //for (auto& player : game_args_.players) {
    //    // Create the player entity
    //    SpawnPoint player_spawn = map_.spawn_points[player.player_idx];
    //    Transform spawn_transform;
    //    spawn_transform.position.x = player_spawn.point.x;
    //    spawn_transform.position.y = player_spawn.point.y;
    //    Path p;
    //    EntityHandle handle = world_.add<ServerArchetypeId::Champion>(
    //        spawn_transform, p, player.team, player_spawn
    //    );
    //    // Emit spawn packet for clients
    //    SpawnCommand c;
    //    c.entity = player.character;
    //    c.position = spawn_transform;
    //    c.server_handle = handle;
    //    SpawnPacket pkt;
    //    pkt.setData(c);
//
    //    net_adapter_->sendPacket(&pkt, Channel::RELIABLECOMMANDS, {});
//
    //    // Register ownership to client
    //    EntityOwnershipPacket ownership_pkt;
    //    EntityOwnershipUpdate ownership_updt {handle};
    //    ownership_pkt.setData(ownership_updt);
    //    DEBUG_LOG("Mapping " << player.account << " to " << handle.eid << handle.gen);
    //    account_entity_map_.insert(player.account, handle);
    //    auto target_peer = client_auth_manager_->getMap().find(player.account);
    //    assert(target_peer && "Peer not assigned when sending ownership");
    //    net_adapter_->sendPacket(&ownership_pkt, Channel::RELIABLECOMMANDS, {*target_peer});
    //}
}


void Server::simulate() {
    initialise();

    running = true;
    DEBUG_LOG("---RUNNING---");

    using clock = std::chrono::steady_clock;

    constexpr auto snapshot_interval = std::chrono::milliseconds(500); // 20 snapshots/sec
    auto next_snapshot_time = clock::now();

    while (running) {
        packet_manager_->pump();

        client_command_system_.update(world_, account_entity_map_);

        const auto now = clock::now();

        if (now >= next_snapshot_time) {
            state_snapshot_system_.update(world_, *net_adapter_);

            next_snapshot_time = now + snapshot_interval;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}