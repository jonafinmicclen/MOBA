#include "GameClient/GameClient.hpp"


GameClient::GameClient() {
 
    // Initialise everything, order matters
    camera = std::make_unique<Camera>();
    cameraController = std::make_unique<CameraController>(camera.get());
    inputManager = std::make_unique<InputManager>();
    game = std::make_unique<Game>();

    exitListener = std::make_unique<ExitListener>(&running);
    inputManager->AddListener(exitListener.get());

    loadAssets();
    initialiseRenderer();    
}

void GameClient::loadAssets() {
    // Hard coded launch parameters, will later be argc ... of --characers ....
    // Only one character for now but obviousl more later
    // Actually this should be recieved from server and not locally
    characterArgs = {"Naren", "Map", "Summoners Rift"};

    // this may be re-used once args are passed into charargs
    for (auto& chararg : characterArgs) {
        // Load in res manager
        auto path = assetDatabase.Get(chararg).asset_path;
        resourceManager.loadAsset(path, chararg);

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
        Render();

    }
} 