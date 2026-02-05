#include "Server/Server.hpp"

void Server::initialise() {
    parseGameArgsJSON();
    netServer = std::make_unique<NetServer>(10);    // Max clients 10
    game = std::make_unique<Game>();
    resourceManager = std::make_unique<ResourceManager>();
    packetDistributor = std::make_unique<PacketDistributor>();
    parseGameArgsJSON();
}


void Server::parseGameArgsJSON() {
    std::ifstream f("RuntimeData/game_args.json");
    json game_args = json::parse(f);

    // Load map
    game->setMap(game_args["map"]);
    resourceManager->loadAsset(game_args["map"]);

    for (auto character : game_args["characters"]) {
        resourceManager->loadAsset(character);
    }
    for (auto& [hash, character] : game_args["players"].items()) {
        std::string playerHash = hash;                  // "hash1"
        std::string characterName = character.get<std::string>(); // "Naren"
        player_hash_character_name[playerHash] = characterName;
    }
}

void Server::beginSimulation() {
    running = true;
    while (running) {
        netServer->pollEvents();
        while (auto packet = netServer->popPacket(PacketFlag::GAMEPLAY)) {
            packetDistributor->dispatch(packet);
        }
    }
}