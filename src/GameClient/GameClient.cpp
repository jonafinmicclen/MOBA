#include "GameClient/GameClient.hpp"


GameClient::GameClient() {
 
    // Initialise everything, order matters
    resourceManager = std::make_unique<ResourceManager>();
    inputManager = std::make_unique<InputManager>();
    renderer = std::make_unique<Renderer>(resourceManager.get(), 1920, 1080);
    game = std::make_unique<Game>();
    exitListener = std::make_unique<ExitListener>(&running);

    inputManager->AddListener(exitListener.get());


    // Hard coded launch parameters, will later be argc ... of --characers ....
    // Only one character for now but obviousl more later
    characterArgs = {"Naren", "Map"};

    // this may be re-used once args are passed into charargs
    for (auto chararg : characterArgs) {
        auto path = assetDatabase.Get(chararg).asset_path;
        resourceManager->loadAsset(path, chararg);
    }


}

void GameClient::Run() {
    while (running) {
        inputManager->Update();
        renderer->Render();
    }
}