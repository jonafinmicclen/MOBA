#include "Server/Server.hpp"


void Server::parseGameArgsJSON() {
    std::ifstream f("RuntimeData/game_args.json");
    json game_args = json::parse(f);

    // Load map
    game->setMap(game_args["map"]);
    resourceManager->loadAsset(game_args["map"]);

    for (auto character : game_args["characters"]) {
        resourceManager->loadAsset(character);
    }
}