#include "GameClient/GameClient.hpp"


GameClient::GameClient() {
 
    // Initialise everything, order matters
    camera = std::make_unique<Camera>();
    cameraController = std::make_unique<CameraController>(camera.get());
    inputManager = std::make_unique<InputManager>();
    game = std::make_unique<Game>();

    std::string server_address = "localhost";
    netClient = std::make_unique<NetClient>(server_address.c_str(), 8080);    // Hardcoded address

    exitListener = std::make_unique<ExitListener>(&running);
    inputManager->AddListener(exitListener.get());

    initialiseRenderer();    

    init_server_connection();    
}

void GameClient::loadAssets(const json& game_args) {

    // Load map
    resourceManager.loadAsset(game_args["map"]);
    game->setMap(game_args["map"]);

    // Load player character
    resourceManager.loadAsset(game_args["active_character"]);

    // Load everything else
    for (auto& asset : game_args["other_assets"]) {
        // Load in res manager
        auto path = assetDatabase.Get(asset).asset_path;
        resourceManager.loadAsset(path, asset);

    }



}

void GameClient::initialiseRenderer() {
    renderer = std::make_unique<Renderer>(window_width, window_height);
    renderer->setCamera(camera.get());

    for (auto& chararg : characterArgs) {
        // Upload to renderer
        Asset* asset = resourceManager.getAsset(chararg);
        renderer->uploadAssetMesh(asset); // only upload once
    }
}

void GameClient::Render() {
    renderer->beginRender();

    renderer->testMesh(translation);
    for (const ObjectState* obj_state : game->getStates()) {
        glm::mat4 transform = obj_state->transform.toMat4();
        renderer->drawMesh(obj_state->name, transform);
    }

    renderer->endRender();
}

void GameClient::Run() {
    while (running) {

        cameraController->update(window_width, window_height);
        inputManager->Update();
        netClient->pollEvents();
        Render();

    }
} 

void GameClient::init_server_connection() {
    while (!netClient->is_connected()) {
        netClient->connectServer();
    }
    DEBUG_LOG("Connected with server");

    auto packet = std::make_unique<ClientAuthenticationPacket>();

    // Locally saved authentication packet
    packet->json_from_file("RuntimeData/auth_ctos.json");

    // Construct message for authentication.
    Message message;
    message.packet = std::move(packet);
    message.header.flag = PacketFlag::GAMEPLAY;

    DEBUG_LOG("Sending auth packet");

    netClient->push_outgoing_packet(message);
    netClient->send_packet_queue();

    DEBUG_LOG("Clearing packets until GAME_ARGS_PACKET");
    for (;;) {
        netClient->pollEvents();
        auto packet = netClient->popPacketQueue();

        if (packet)  {
            DEBUG_LOG("Dispatching packet");
            packetDistributor.dispatch(packet, 0);
            break;
        }
    }

}


void GameClient::processGameArgsPacket(const GameArgsPacket& pkt) {
    loadAssets(pkt.get_json());
}

void GameClient::initialisePacketDistributors() {
    packetDistributor.on<GameArgsPacket>(
        PacketType::GAME_ARGS_PACKET, 
        [this](const GameArgsPacket& pkt, const uint8_t id) {
            processGameArgsPacket(pkt);
        }
    );
}
