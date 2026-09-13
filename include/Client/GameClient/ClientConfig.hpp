#pragma once

#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

#include "Client/Input/ClientButton.hpp"

// Client-side settings not tied to any specific map/game session - window
// size, camera feel, and keybindings. Lives in RuntimeData for now (matches
// game_args.json/auth_ctos.json's convention); may move somewhere more
// permanent later.
struct ClientConfig {
    int window_width = 1280;
    int window_height = 720;
    float pan_speed_divisor = 13000.0f; // larger = slower panning
    float zoom_distance = 4.58257569f;  // camera's distance from its target

    // Button -> command, inverted from the config's command: button layout
    // (see "keybindings" in client_config.json) since input handling needs
    // to look up "what does this button do", not the other way around.
    std::unordered_map<ClientButton, ClientCommand> keybindings;

    static ClientConfig load(const std::string& path) {
        std::ifstream file(path);
        if (!file) {
            throw std::runtime_error("Failed to open client config: " + path);
        }

        nlohmann::json j;
        file >> j;

        ClientConfig config;
        config.window_width = j.at("window").at("width").get<int>();
        config.window_height = j.at("window").at("height").get<int>();
        config.pan_speed_divisor = j.at("camera").at("panSpeedDivisor").get<float>();
        config.zoom_distance = j.at("camera").at("zoomDistance").get<float>();

        if (j.contains("keybindings")) {
            for (const auto& [command_name, button_value] : j.at("keybindings").items()) {
                ClientCommand command = clientCommandFromString(command_name);
                ClientButton button = clientButtonFromString(button_value.get<std::string>());
                config.keybindings[button] = command;
            }
        }

        return config;
    }

    // Writes the current values back out - called on shutdown (see
    // GameClient::run()) so live-tuned settings (scroll zoom, the pan-speed
    // debug tuner) persist to the next launch instead of resetting.
    void save(const std::string& path) const {
        nlohmann::json j;
        j["window"]["width"] = window_width;
        j["window"]["height"] = window_height;
        j["camera"]["panSpeedDivisor"] = pan_speed_divisor;
        j["camera"]["zoomDistance"] = zoom_distance;

        nlohmann::json kb = nlohmann::json::object();
        for (const auto& [button, command] : keybindings) {
            kb[clientCommandToString(command)] = clientButtonToString(button);
        }
        j["keybindings"] = kb;

        std::ofstream file(path);
        if (!file) {
            throw std::runtime_error("Failed to write client config: " + path);
        }
        file << j.dump(2);
    }
};
