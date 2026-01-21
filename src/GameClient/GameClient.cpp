#include "GameClient/GameClient.hpp"


GameClient::GameClient() {
 
    // Initialise everything, order matters
    resourceManager = std::make_unique<ResourceManager>();
    inputManager = std::make_unique<InputManager>();
    
    game = std::make_unique<Game>();

    // Hard coded launch parameters, will later be argc ... of --characers ....
    // Only one character for now but obviousl more later
    characterArgs = {"Naren", "Map", "Summoners Rift"};

    // this may be re-used once args are passed into charargs
    for (auto& chararg : characterArgs) {
        // Load in res manager
        auto path = assetDatabase.Get(chararg).asset_path;
        resourceManager->loadAsset(path, chararg);

    }

    renderer = std::make_unique<Renderer>(resourceManager.get(), 1920, 1080);

    for (auto& chararg : characterArgs) {
        // Upload to renderer
        Asset* asset = resourceManager->getAsset(chararg);
        renderer->uploadAssetMesh(asset); // only upload once
    }

    // Fully init meshes to renderer
    initRenderer();

    // Init exit listener
    exitListener = std::make_unique<ExitListener>(&running);
    inputManager->AddListener(exitListener.get());

    translationListener = std::make_unique<ArrowKeyMoveListener>(&translation);
    inputManager->AddListener(translationListener.get());



}

void GameClient::initRenderer() {
    
}

void GameClient::Run() {
    while (running) {

        inputManager->Update();
        renderer->beginRender();
            // Translation add it
        renderer->testMesh(translation);

        for (const AssetState* state : game->getState()) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, state->position); // example position
            model = glm::rotate(model, state->rot_angle, state->rot_axis);
            model = glm::scale(model, state->scale);   
            renderer->drawMesh(state->name, model);
        }

        renderer->endRender();
    }
} 