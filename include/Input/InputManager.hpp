// InputManager.hpp
#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <unordered_map>

#include "Input/InputEventTypes.hpp"
#include "Input/IInputListener.hpp"

#include "Debug/debug.hpp"


class InputManager {
public:
    void addListener(IInputListener* listener);
    void update();

private:
    std::unordered_map<InputEventType, std::vector<IInputListener*>> listeners;
    void dispatchEvent(InputEvent& event);
};