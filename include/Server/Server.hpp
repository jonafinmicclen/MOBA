#pragma once


#include <memory>
#include <chrono>
#include <thread>

#include "Core/Networking/Core/Networker.hpp"
#include "Core/Networking/Packets/PacketTypes.hpp"
#include "Core/Networking/Packets/PacketDistributor.hpp"
#include "Core/Networking/NetConfig.hpp"
#include "Core/Networking/PacketManager.hpp"
#include "Core/Adapter/NetAdapter.hpp"
#include "Server/Initialiser/ClientAuthManager.hpp"
#include "Common/Memory/FIFOQueue.hpp"
#include "Common/Memory/BiMap.hpp"
#include "Server/Systems/Input/ClientInputSystem.hpp"
#include "Server/Systems/Movement/PathFollowingSystem.hpp"

#include "Game/Packets/Gameplay/SpawnPacket.hpp"

#include "Game/Packets/Initialiser/EntityOwnershipPacket.hpp"
#include "Game/Packets/Initialiser/ClientAuthenticationPacket.hpp"
#include "Game/Packets/Initialiser/GameArgsPacket.hpp"
#include "Server/Systems/Distribution/StateSnapshotSystem.hpp"
#include "Common/Player/AccountID.hpp"

#include "Server/ClientState.hpp"

#include "Core/Networking/Session/PeerDirectory.hpp"

#include "Assets/ResourceManager.hpp"

#include <optional>
#include <unordered_map>
#include <fstream>
#include <nlohmann/json.hpp>
#include "Game/Control/PlayerCommand.hpp"

#include "Game/Placeholder/PlaceholderMapDef.hpp"

#include "Server/ServerWorld.hpp"

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
    PathFollowingSystem path_following_system_;

    bool running = false;
};