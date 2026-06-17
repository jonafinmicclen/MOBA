#pragma once

#include <memory>

#include <nlohmann/json.hpp>
#include <enet/enet.h>

#include "Common/Debug/debug.hpp"

#include "Client/Renderer/Renderer.hpp"

#include "Assets/ModelData.hpp"
#include "Assets/ResourceManager.hpp"

#include "Client/Input/InputManager.hpp"

#include "Client/GameClient/Camera/Camera.hpp"
#include "Client/GameClient/Camera/CameraController.hpp"

#include "Core/Networking/Core/Networker.hpp"
#include "Core/Networking/Packets/PacketDistributor.hpp"
#include "Core/Networking/NetConfig.hpp"
#include "Core/Networking/PacketManager.hpp"
#include "Core/Networking/Core/NetEventDistributor.hpp"
#include "Game/Packets/Gameplay/ClientCommandPacket.hpp"
#include "Core/Adapter/NetAdapter.hpp"
#include "Client/GameClient/Net/ServerConnectionManager.hpp"

#include "Game/Packets/Initialiser/ClientAuthenticationPacket.hpp"
#include "Game/Packets/Initialiser/GameArgsPacket.hpp"
#include "Client/GameClient/ClientWorld.hpp"
#include "Client/GameClient/Net/GameArgsHandler.hpp"

#include "Client/GameClient/Duplication/DuplicationSystem.hpp"


class GameClient {
public:
    GameClient();

    void run();

private:
    void registerSendInputCommands();
    void render();

    std::optional<Networker> networker_;
    std::optional<NetAdapter> network_adapter_;

    std::optional<NetEventDistributor> network_event_distributor_;
    std::optional<ServerConnectionManager> server_connection_manager_;

    std::optional<PacketDistributor> packet_distributor_;
    std::optional<PacketManager> packet_manager_;

    std::optional<GameArgsHandler> game_args_handler_;

    std::optional<Renderer> renderer_;
    std::optional<Camera> camera_;

    std::optional<CameraController> camera_controller_;
    std::optional<InputManager> input_manager_; 

    std::optional<DuplicationSystem> duplication_system_;

    ClientWorld world_;
    
    int window_width_ = 1920;
    int window_height_ = 1080;

    bool running_ = false;
};