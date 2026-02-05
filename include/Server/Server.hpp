#pragma once


#include <memory>

#include "Networking/Server/NetServer.hpp"
#include "Networking/Packets/PacketTypes.hpp"
#include "Networking/PacketFlag.hpp"
#include "Networking/Packets/PacketDistributor.hpp"

#include "Assets/ResourceManager.hpp"
#include "Game/Game.hpp"

#include <unordered_map>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;


class Server {
    public:
    void simulate() {beginSimulation();}

    private:
    void initialise();
    void parseGameArgsJSON(); // Load from JSON
    void beginSimulation();

    std::unordered_map<std::string, std::string> player_hash_character_name;

    std::unique_ptr<Game> game;
    std::unique_ptr<NetServer> netServer;
    std::unique_ptr<ResourceManager> resourceManager;
    std::unique_ptr<PacketDistributor> packetDistributor;

    bool running;
};