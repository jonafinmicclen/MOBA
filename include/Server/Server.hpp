#pragma once


#include <memory>

#include "Networking/Server/NetServer.hpp"
#include "Networking/Packets/PacketTypes.hpp"
#include "Networking/PacketFlag.hpp"
#include "Networking/Packets/PacketDistributor.hpp"
#include "GameClient/Packets/ClientAuthenticationPacket.hpp"
#include "GameClient/Packets/GameArgsPacket.hpp"

#include "Assets/ResourceManager.hpp"
#include "Game/Game.hpp"

#include <optional>
#include <unordered_map>
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

using user_id = std::string;
using client_id = uint8_t;

class Server {
    public:
    void simulate() {beginSimulation();}

    private:
    void initialise();
    void initialise_packet_listeners();
    void parseGameArgsJSON(); // Load from JSON
    void beginSimulation();

    void authenticate_client(const ClientAuthenticationPacket& pkt, const uint8_t id);
    void initialise_client(const std::string user_id, const uint8_t client_id);

    std::unordered_map<user_id, std::string> character_by_user_id;
    std::unordered_map<client_id, user_id> user_id_by_client_id;

    std::unique_ptr<Game> game;
    std::unique_ptr<NetServer> netServer;
    std::unique_ptr<PacketDistributor> packetDistributor;

    bool running = false;
};