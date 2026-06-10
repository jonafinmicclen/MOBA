#pragma once


#include <memory>

#include "Networking/Core/Networker.hpp"
#include "Networking/Packets/PacketTypes.hpp"
#include "Networking/Packets/PacketDistributor.hpp"
#include "Networking/NetConfig.hpp"
#include "Networking/PacketManager.hpp"
#include "Adapter/NetAdapter.hpp"
#include "Server/ClientAuthManager.hpp"
#include "Common/Memory/FIFOQueue.hpp"
#include "Common/Memory/BiMap.hpp"
#include "Server/CommandHandler.hpp"
#include "Game/ECS/Systems/ClientCommandSystem.hpp"

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
#include "Game/Control/PlayerCommand.hpp"

#include "Game/Placeholder/PlaceholderMapDef.hpp"

using json = nlohmann::json;

using user_id = std::string;
using client_id = uint8_t;
using ClientCommandQueue = FIFOQueue<ClientCommand>;

class Server {
public:
    void simulate();
    void exit() {running = false;}
    void pushClientCommandQueue(ClientCommand c) {client_command_system_.getQueue().push(c);}

private:
    void initialise();
    void loadConfig();

    std::unique_ptr<Game> game_;

    std::optional<Networker> net_server_;
    std::optional<NetAdapter> net_adapter_;

    std::optional<PacketDistributor> packet_distributor_;
    std::optional<PacketManager> packet_manager_;

    std::optional<ClientAuthManager> client_auth_manager_;
    std::optional<CommandHandler> client_command_handler_;
    
    std::optional<GameArgs> game_args_;
    
    // Runtime systems
    ClientCommandSystem client_command_system_;

    bool running = false;
    BiMap<AccountHash, EntityHandle> account_entity_map_;
};