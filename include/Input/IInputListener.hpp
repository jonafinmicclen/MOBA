#pragma once

#include <SDL2/SDL.h>

#include "Input/InputEventTypes.hpp"


class IInputListener {
    public:
    virtual ~IInputListener() = default;
    virtual void OnInputEvent(InputEvent& event) = 0;
};