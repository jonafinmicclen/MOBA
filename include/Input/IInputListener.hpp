#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <iostream>

#include "Input/InputEventTypes.hpp"


class IInputListener {
public:
    virtual ~IInputListener() = default;
    virtual void OnInputEvent(InputEvent& event) = 0;
    virtual std::vector<InputEventType> getInterests() = 0;
};