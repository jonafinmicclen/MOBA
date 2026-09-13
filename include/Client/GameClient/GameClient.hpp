#pragma once

#include <memory>

#include <nlohmann/json.hpp>
#include <enet/enet.h>

#include "Common/Debug/debug.hpp"
#include "Common/Debug/DebugOverlaySystem.hpp"

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

#include "Client/GameClient/MapPlacement.hpp"
#include "Client/GameClient/ClientConfig.hpp"
#include "Client/Input/ClientButton.hpp"

#ifdef DEBUG
#include "Client/Debug/WalkableGridOverlay.hpp"
#include "Client/Debug/DebugPanSpeedController.hpp"
#endif


class GameClient {
public:
    GameClient();

    void run();

private:
    void registerSendInputCommands();
    void handleButtonPress(ClientButton btn, ScreenSpacePos screen_pos);
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
    DebugOverlaySystem debug_overlay_system_;

#ifdef DEBUG
    // F12 toggles all dev/debug visuals and tuning controls at once (see
    // registerSendInputCommands and render()).
    bool dev_mode_enabled_ = true;

    std::optional<WalkableGridOverlay> debug_walkable_grid_overlay_;
    // Live-tunable via Page Up/Page Down (see registerSendInputCommands) -
    // multiplies the purple grid's rendered size about the world origin so
    // the right map_from_world.scale can be found by eye against the
    // visible ground, without rebuilding the mesh each try. 1.0 = no change.
    float debug_grid_scale_ = 1.0f;
    // Live-tunable via Home/End - added on top of kMapGroundHeight. The map
    // mesh's true visible floor isn't necessarily at its Transform's
    // position.z (the model's local origin may sit somewhere else), so the
    // grid legitimately needs its own height, found separately by eye.
    // Defaults to cancelling kMapGroundHeight out entirely (found total
    // height 0) - the model's local origin is offset from its visible floor
    // by exactly kMapGroundHeight.
    float debug_grid_height_offset_ = -kMapGroundHeight;

    std::optional<DebugPanSpeedController> debug_pan_speed_controller_;
#endif

    ClientWorld world_;

    static constexpr const char* kClientConfigPath = "RuntimeData/client_config.json";
    // Read on startup (constructor), written back on shutdown (see run())
    // so live-tuned settings (scroll zoom, the pan-speed debug tuner)
    // persist across launches instead of resetting.
    ClientConfig client_config_;

    int window_width_ = 1280;
    int window_height_ = 720;

    // Toggled by L (see registerSendInputCommands) - confines the cursor to
    // the window so it can't wander onto another monitor while panning.
    bool mouse_locked_ = false;

    bool running_ = false;
};