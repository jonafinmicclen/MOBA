#pragma once


#include <memory>

#include "Networking/Server/NetServer.hpp"

#include "Assets/ResourceManager.hpp"
#include "Game/Game.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


class Server {
    public:
    void simulate();

    private:
    void initialise();
    void parseGameArgsJSON(); // Load from JSON
    void beginSimulation();

    std::unique_ptr<Game> game;
    std::unique_ptr<NetServer> netServer;
    std::unique_ptr<ResourceManager> resourceManager;
};