#pragma once

#include <SDL2/SDL.h>

#include "Common/Coordinates/ScreenSpacePos.hpp"


enum class InputEventType {
    KeyDown,
    KeyUp,
    MouseButtonDown,
    MouseButtonUp
};

struct InputEvent {
    InputEventType type;
    SDL_Keycode key;
    ScreenSpacePos mousePos;
    Uint8 mouseButton;
};