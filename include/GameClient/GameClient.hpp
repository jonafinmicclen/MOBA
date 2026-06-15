#pragma once

#include <memory>

#include <nlohmann/json.hpp>
#include <enet/enet.h>

#include "Debug/debug.hpp"

#include "Renderer/Renderer.hpp"

#include "Game/ObjectState.hpp"
#include "Game/Maps/MapFactory.hpp"
#include "Game/Maps/SummonersRift.hpp"

#include "Assets/ModelData.hpp"
#include "Assets/ResourceManager.hpp"

#include "Input/InputManager.hpp"
#include "Input/Listeners/ExitListener.hpp"
#include "Input/Listeners/ArrowKeyMoveListener.hpp"

#include "GameClient/Camera/Camera.hpp"
#include "GameClient/Camera/CameraController.hpp"

#include "Networking/Core/Networker.hpp"
#include "Networking/Packets/PacketDistributor.hpp"
#include "Networking/NetConfig.hpp"
#include "Networking/PacketManager.hpp"
#include "Networking/Core/NetEventDistributor.hpp"
#include "Networking/Packets/ClientCommandPacket.hpp"
#include "Adapter/NetAdapter.hpp"
#include "GameClient/Net/ServerConnectionManager.hpp"

#include "GameClient/Packets/ClientAuthenticationPacket.hpp"
#include "GameClient/Packets/GameArgsPacket.hpp"
#include "EntityComponentSystem/Worlds/ClientWorld.hpp"
#include "GameClient/Net/GameArgsHandler.hpp"

#include "GameClient/Duplication/DuplicationSystem.hpp"


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