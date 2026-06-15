#pragma once


#include <memory>
#include <chrono>
#include <thread>

#include "Networking/Core/Networker.hpp"
#include "Networking/Packets/PacketTypes.hpp"
#include "Networking/Packets/PacketDistributor.hpp"
#include "Networking/NetConfig.hpp"
#include "Networking/PacketManager.hpp"
#include "Adapter/NetAdapter.hpp"
#include "Server/ClientServices/ClientAuthManager.hpp"
#include "Common/Memory/FIFOQueue.hpp"
#include "Common/Memory/BiMap.hpp"
#include "EntityComponentSystem/Systems/ClientInputSystem.hpp"

#include "Game/Packets/SpawnPacket.hpp"

#include "Server/ClientServices/AccountCharacterBiMap.hpp"
#include "Game/Packets/EntityOwnershipPacket.hpp"
#include "GameClient/Packets/ClientAuthenticationPacket.hpp"
#include "GameClient/Packets/GameArgsPacket.hpp"
#include "EntityComponentSystem/Systems/StateSnapshotSystem.hpp"
#include "Common/Player/AccountID.hpp"

#include "Server/ClientServices/ClientState.hpp"

#include "Networking/Session/PeerDirectory.hpp"

#include "Assets/ResourceManager.hpp"

#include <optional>
#include <unordered_map>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Game/Control/PlayerCommand.hpp"

#include "Game/Placeholder/PlaceholderMapDef.hpp"

#include "EntityComponentSystem/Worlds/ServerWorld.hpp"

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

    ClientState client_state_;

    // Game state
    ServerWorld world_;
    MapDef map_;
    GameArgs game_args_ = GameArgs("RuntimeData/game_args.json");

    // Networking
    std::optional<Networker> net_server_;
    std::optional<NetAdapter> net_adapter_;
    std::optional<PacketManager> packet_manager_;
    std::optional<PacketDistributor> packet_distributor_;

    // Services
    std::optional<ClientAuthSystem> client_auth_system_;
    std::optional<ClientInputSystem> client_input_system_;
    StateSnapshotSystem state_snapshot_system_;

    bool running = false;
};