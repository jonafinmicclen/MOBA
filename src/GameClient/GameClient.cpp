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
    game_ = std::make_unique<Game>();

    exit_listener_.emplace(&running_);
    input_manager_->AddListener(&*exit_listener_);

    game_args_handler_.emplace(AssetDatabase::instance(), ResourceManager::instance(), *renderer_, *game_, *packet_distributor_);

    renderer_.emplace(window_width_, window_height_);
    renderer_->setCamera(&*camera_);

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

    for (const ObjectState* obj_state : game_->getStates()) {
        glm::mat4 transform = obj_state->transform.toMat4();
        renderer_->drawMesh(obj_state->name, transform);
    }

    renderer_->endRender();
}

void GameClient::run() {
    running_ = true;
    while (running_) {

        packet_manager_->pump();
        network_event_distributor_->pump();
        camera_controller_->update(window_width_, window_height_);
        input_manager_->Update();
        render();

    }
} 