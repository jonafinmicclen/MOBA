#pragma once

#include <optional>
#include <stdexcept>
#include <string>

#include <SDL2/SDL.h>

#include "Game/Control/PlayerCommand.hpp"

// Physical input identifier - a key or mouse button. Client-only, never
// sent over the network (ClientCommand is what actually gets sent). The
// mapping from ClientButton to ClientCommand lives in client_config.json
// ("keybindings"), loaded by ClientConfig.
enum class ClientButton : uint8_t {
    LEFT_CLICK,
    RIGHT_CLICK,
    T,
    Q,
    W,
    E,
    R,
    D,
    F,
    ITEMSLOT1,
    ITEMSLOT2,
    ITEMSLOT3,
    ITEMSLOT4,
    ITEMSLOT5,
    ITEMSLOT6
};

inline ClientButton clientButtonFromString(const std::string& s) {
    if (s == "LEFT_CLICK") return ClientButton::LEFT_CLICK;
    if (s == "RIGHT_CLICK") return ClientButton::RIGHT_CLICK;
    if (s == "T") return ClientButton::T;
    if (s == "Q") return ClientButton::Q;
    if (s == "W") return ClientButton::W;
    if (s == "E") return ClientButton::E;
    if (s == "R") return ClientButton::R;
    if (s == "D") return ClientButton::D;
    if (s == "F") return ClientButton::F;
    if (s == "ITEMSLOT1") return ClientButton::ITEMSLOT1;
    if (s == "ITEMSLOT2") return ClientButton::ITEMSLOT2;
    if (s == "ITEMSLOT3") return ClientButton::ITEMSLOT3;
    if (s == "ITEMSLOT4") return ClientButton::ITEMSLOT4;
    if (s == "ITEMSLOT5") return ClientButton::ITEMSLOT5;
    if (s == "ITEMSLOT6") return ClientButton::ITEMSLOT6;
    throw std::runtime_error("Unknown ClientButton in config: " + s);
}

inline ClientCommand clientCommandFromString(const std::string& s) {
    if (s == "MOVE") return ClientCommand::MOVE;
    if (s == "SPAWN_DUMMY") return ClientCommand::SPAWN_DUMMY;
    throw std::runtime_error("Unknown ClientCommand in config: " + s);
}

// Reverse of the above - used when writing the config back out (see
// ClientConfig::save), so keybindings round-trip through a save/load cycle.
inline std::string clientButtonToString(ClientButton b) {
    switch (b) {
        case ClientButton::LEFT_CLICK:  return "LEFT_CLICK";
        case ClientButton::RIGHT_CLICK: return "RIGHT_CLICK";
        case ClientButton::T:           return "T";
        case ClientButton::Q:           return "Q";
        case ClientButton::W:           return "W";
        case ClientButton::E:           return "E";
        case ClientButton::R:           return "R";
        case ClientButton::D:           return "D";
        case ClientButton::F:           return "F";
        case ClientButton::ITEMSLOT1:   return "ITEMSLOT1";
        case ClientButton::ITEMSLOT2:   return "ITEMSLOT2";
        case ClientButton::ITEMSLOT3:   return "ITEMSLOT3";
        case ClientButton::ITEMSLOT4:   return "ITEMSLOT4";
        case ClientButton::ITEMSLOT5:   return "ITEMSLOT5";
        case ClientButton::ITEMSLOT6:   return "ITEMSLOT6";
    }
    throw std::runtime_error("Unhandled ClientButton");
}

inline std::string clientCommandToString(ClientCommand c) {
    switch (c) {
        case ClientCommand::MOVE:        return "MOVE";
        case ClientCommand::SPAWN_DUMMY: return "SPAWN_DUMMY";
    }
    throw std::runtime_error("Unhandled ClientCommand");
}

// nullopt for keys/buttons with no ClientButton equivalent (most of the
// keyboard) - callers should just ignore those.
inline std::optional<ClientButton> clientButtonFromKey(SDL_Keycode key) {
    switch (key) {
        case SDLK_t: return ClientButton::T;
        case SDLK_q: return ClientButton::Q;
        case SDLK_w: return ClientButton::W;
        case SDLK_e: return ClientButton::E;
        case SDLK_r: return ClientButton::R;
        case SDLK_d: return ClientButton::D;
        case SDLK_f: return ClientButton::F;
        case SDLK_1: return ClientButton::ITEMSLOT1;
        case SDLK_2: return ClientButton::ITEMSLOT2;
        case SDLK_3: return ClientButton::ITEMSLOT3;
        case SDLK_4: return ClientButton::ITEMSLOT4;
        case SDLK_5: return ClientButton::ITEMSLOT5;
        case SDLK_6: return ClientButton::ITEMSLOT6;
        default: return std::nullopt;
    }
}

inline std::optional<ClientButton> clientButtonFromMouseButton(Uint8 sdl_button) {
    switch (sdl_button) {
        case SDL_BUTTON_LEFT: return ClientButton::LEFT_CLICK;
        case SDL_BUTTON_RIGHT: return ClientButton::RIGHT_CLICK;
        default: return std::nullopt;
    }
}
