#include "Client/GameClient/GameClient.hpp"


GameClient::GameClient() {
    client_config_ = ClientConfig::load(kClientConfigPath);
    window_width_ = client_config_.window_width;
    window_height_ = client_config_.window_height;

    NetConfig cfg(
        2,
        3,
        nullptr,
        8080
    );

    networker_.emplace(cfg);
    network_adapter_.emplace(*networker_);
    packet_distributor_.emplace();
    packet_manager_.emplace(*packet_distributor_, *networker_);

    camera_.emplace(client_config_.zoom_distance);
    input_manager_.emplace();
    camera_controller_.emplace(&*camera_, client_config_.pan_speed_divisor, *input_manager_);

    input_manager_->addListener(InputEventType::Exit, [this](const InputEvent& e) {
        running_ = false;
    });

    registerSendInputCommands();

#ifdef DEBUG
    debug_pan_speed_controller_.emplace(*camera_controller_, *input_manager_, dev_mode_enabled_);
#endif

    duplication_system_.emplace(world_, *packet_distributor_, *camera_);

    renderer_.emplace(window_width_, window_height_);
    renderer_->setCamera(&*camera_);

    ResourceManager::instance().init(&*renderer_);

    // Map data (walkable grid, spawn points, camera pan bounds) isn't known
    // until the server sends GameArgsPacket - GameArgsHandler loads the
    // MapDef and sets the camera's pan bounds once that arrives (see
    // applyGameArgs). The debug overlay below is built lazily in render()
    // once that data is available.
    game_args_handler_.emplace(AssetDatabase::instance(), ResourceManager::instance(), *renderer_, *camera_, *packet_distributor_, world_);

    network_event_distributor_.emplace(*networker_);

    server_connection_manager_.emplace(
        "RuntimeData/auth_ctos.json",
        *network_adapter_,
        *network_event_distributor_,
        ConnectCmd({"localhost", 8080})
    );

    networker_->start();
    server_connection_manager_->connect();
}


void GameClient::render() {
    renderer_->beginRender();

    world_.queryColumns<Transform, MeshId>([this](std::span<Transform> t, std::span<MeshId> id){
        for (size_t i = 0; i < t.size(); ++i) {
            renderer_->drawMesh(id[i], t[i].toMat4());
        }
    });

#ifdef DEBUG
    if (dev_mode_enabled_) {
        if (!debug_walkable_grid_overlay_) {
            // Map data only exists once GameArgsHandler has processed
            // GameArgsPacket - build the overlay's mesh the first frame
            // it's available rather than at construction time.
            if (const MapDef* map = game_args_handler_->getMapDef()) {
                debug_walkable_grid_overlay_.emplace(map->walkable_area, map->map_from_world);
            }
        }

        if (debug_walkable_grid_overlay_) {
            // The overlay draws with depth testing off, so it's always
            // visible on top regardless of the map's actual terrain - no
            // artificial lift needed to dodge z-fighting.
            DEBUG_STAT("WalkableGridOverlay scale multiplier", debug_grid_scale_);
            if (const MapDef* map = game_args_handler_->getMapDef()) {
                DEBUG_STAT("Corrected nav.transform.scale", map->map_from_world.scale.x.toFloat() / debug_grid_scale_);
            }

            float height = kMapGroundHeight + debug_grid_height_offset_;
            DEBUG_STAT("WalkableGridOverlay height (Z)", height);

            float aspect = static_cast<float>(window_width_) / static_cast<float>(window_height_);
            debug_walkable_grid_overlay_->draw(camera_->getView(), camera_->getProjection(aspect), height, debug_grid_scale_);
        }
    }
#endif

    renderer_->endRender();
}

void GameClient::run() {
    running_ = true;
    while (running_) {

        packet_manager_->pump();
        network_event_distributor_->pump();
        camera_controller_->update(window_width_, window_height_);
        input_manager_->update();
        render();
        debug_overlay_system_.update();

    }

    // Persist whatever the player ended up with (scroll zoom, the pan-speed
    // debug tuner) so the next launch picks up where this one left off.
    client_config_.pan_speed_divisor = camera_controller_->getPanSpeedDivisor();
    client_config_.zoom_distance = camera_->getZoomDistance();
    client_config_.save(kClientConfigPath);
}

void GameClient::registerSendInputCommands() {
#ifdef DEBUG
    input_manager_->addListener(InputEventType::KeyDown,
        [this](const InputEvent& e) {
            if (e.key == SDLK_F12) {
                dev_mode_enabled_ = !dev_mode_enabled_;
                DEBUG_LOG("Dev mode " << (dev_mode_enabled_ ? "enabled" : "disabled"));
                return;
            }

            if (!dev_mode_enabled_) return;

            if (e.key == SDLK_PAGEUP) {
                debug_grid_scale_ += 0.05f;
            } else if (e.key == SDLK_PAGEDOWN) {
                debug_grid_scale_ -= 0.05f;
            } else if (e.key == SDLK_HOME) {
                debug_grid_height_offset_ += 0.5f;
            } else if (e.key == SDLK_END) {
                debug_grid_height_offset_ -= 0.5f;
            }
        }
    );
#endif

    input_manager_->addListener(InputEventType::KeyDown,
        [this](const InputEvent& e) {
            if (e.key == SDLK_l) {
                mouse_locked_ = !mouse_locked_;
                renderer_->setMouseLocked(mouse_locked_);
                DEBUG_LOG("Mouse lock " << (mouse_locked_ ? "enabled" : "disabled"));
            }
        }
    );

    // Generic keybinding-driven command dispatch: any bound button (mouse
    // or key) resolves through client_config_.keybindings ("keybindings" in
    // client_config.json) to a ClientCommand, which is what actually gets
    // sent - the server only ever sees the command, never which physical
    // button triggered it.
    input_manager_->addListener(InputEventType::KeyDown,
        [this](const InputEvent& e) {
            if (auto btn = clientButtonFromKey(e.key)) {
                handleButtonPress(*btn, e.mousePos);
            }
        }
    );

    input_manager_->addListener(InputEventType::MouseButtonUp,
        [this](const InputEvent& e) {
            if (auto btn = clientButtonFromMouseButton(e.mouseButton)) {
                handleButtonPress(*btn, e.mousePos);
            }
        }
    );
}

void GameClient::handleButtonPress(ClientButton btn, ScreenSpacePos screen_pos) {
    auto it = client_config_.keybindings.find(btn);
    if (it == client_config_.keybindings.end()) {
        return; // unbound button - nothing configured to happen
    }

    ClientInput c;
    c.command = it->second;
    c.mouse_pos = camera_->screenToWorldOnMap(screen_pos, window_width_, window_height_);
    c.release = true;

    ClientInputPacket p;
    p.getData() = c;
    network_adapter_->sendPacket(&p, Channel::RELIABLECOMMANDS, {});
}
