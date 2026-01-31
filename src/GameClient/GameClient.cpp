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


    renderer = std::make_unique<Renderer>(resourceManager.get(), window_width, window_height);

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
        MoveCameraTowardsMouse();

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

void GameClient::MoveCameraTowardsMouse() {
    int x;
    int y;
    float dx = 0;
    float dy = 0;
    const float inverse_threshold = 2.5f;
    const float inverse_sensitivity = 10000.0f;
    Uint32 buttons = SDL_GetMouseState(&x, &y);

    int x_from_centre = x - window_width / 2;
    int y_from_centre = y - window_height / 2;

    if (abs(x_from_centre) > window_width/inverse_threshold) {
        DEBUG_LOG("x threshold met");
        dx += (float)x_from_centre / inverse_sensitivity;
    }

    if (abs(y_from_centre) > window_height/inverse_threshold) {
        DEBUG_LOG("y threshold met");
        dy -= (float)y_from_centre / inverse_sensitivity;
    }
    renderer->moveCamera2D({dx, dy});

}