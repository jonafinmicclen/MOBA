#pragma once

#include <memory>

#include <nlohmann/json.hpp>
#include <enet/enet.h>

#include "Debug/debug.hpp"

#include "Renderer/Renderer.hpp"

#include "Game/Game.hpp"
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

#include "GameClient/Packets/ClientAuthenticationPacket.hpp"
#include "GameClient/Packets/GameArgsPacket.hpp"


class GameClient {
public:
    GameClient();
    void Run();

private:
    std::unique_ptr<InputManager> inputManager; 
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Game> game;
    std::unique_ptr<Networker> networker_;

    std::unique_ptr<Camera> camera;
    std::unique_ptr<CameraController> cameraController;

    std::unique_ptr<ExitListener>  exitListener;

    AssetDatabase& assetDatabase = AssetDatabase::instance();
    ResourceManager& resourceManager = ResourceManager::instance();

    PacketDistributor packetDistributor;

    std::vector<std::string> characterArgs;

    glm::vec3 translation = {0.0f, 0.0f, 0.0f};

    void initialiseRenderer();
    void loadAssets(const json& game_args);
    void Render();
    void init_server_connection();

    void processGameArgsPacket(GameArgsPacket& pkt);

    void initialisePacketDistributors();

    int window_width = 1920;
    int window_height = 1080;

    bool running = true;
};