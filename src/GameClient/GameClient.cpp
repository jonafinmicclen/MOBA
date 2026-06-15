#include "GameClient/GameClient.hpp"


GameClient::GameClient() {

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

    camera_.emplace();
    camera_controller_.emplace(&*camera_);
    input_manager_.emplace();

    input_manager_->addListener(InputEventType::Exit, [this](const InputEvent& e) {
        running_ = false;
    });

    registerSendInputCommands();
    
    duplication_system_.emplace(world_, *packet_distributor_);

    renderer_.emplace(window_width_, window_height_);
    renderer_->setCamera(&*camera_);

    ResourceManager::instance().init(&*renderer_);

    game_args_handler_.emplace(AssetDatabase::instance(), ResourceManager::instance(), *renderer_, *packet_distributor_, world_);

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

    }
} 

void GameClient::registerSendInputCommands() {
    input_manager_->addListener(InputEventType::MouseButtonUp,
        [this](const InputEvent& e) {
            ClientInput c;
            c.btn = ClientInputButton::RIGHT_CLICK;
            c.mouse_pos = camera_->screenToWorldOnMap(e.mousePos, window_width_, window_height_);
            DEBUG_LOG("world mouse x: " << c.mouse_pos.x << ", y: "<< c.mouse_pos.y);
            c.release = true;
            ClientInputPacket p;
            p.getData() = c;
            network_adapter_->sendPacket(&p, Channel::RELIABLECOMMANDS, {});
        }
    );
}