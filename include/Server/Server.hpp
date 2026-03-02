#pragma once


#include <memory>

#include "Networking/Core/Networker.hpp"
#include "Networking/Packets/PacketTypes.hpp"
#include "Networking/Packets/PacketDistributor.hpp"
#include "Networking/NetConfig.hpp"
#include "Networking/PacketManager.hpp"
#include "Adapter/NetAdapter.hpp"

#include "Server/AccountCharacterBiMap.hpp"

#include "GameClient/Packets/ClientAuthenticationPacket.hpp"
#include "GameClient/Packets/GameArgsPacket.hpp"

#include "Common/Player/AccountID.hpp"

#include "Networking/Session/PeerDirectory.hpp"

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
    void simulate();
    void exit() {running = false;}

private:
    void initialise();
    void loadConfig();

    std::optional<NetAdapter> net_adapter_;

    std::optional<PacketDistributor> packet_distributor_;
    std::optional<PacketManager> packet_manager_;

    std::optional<Networker> net_server_;
    std::unique_ptr<Game> game_;

    std::optional<GameArgs> game_args_ = std::nullopt;
    

    bool running = false;
};