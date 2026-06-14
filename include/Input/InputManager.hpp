#pragma once

#include <SDL2/SDL.h>

#include <functional>
#include <unordered_map>
#include <vector>

#include "Input/InputEventTypes.hpp"
#include "Debug/debug.hpp"

class InputManager {
public:
    using InputCallback = std::function<void(const InputEvent&)>;

    void addListener(InputEventType type, InputCallback callback);
    void addListener(std::initializer_list<InputEventType> types, InputCallback callback);
    void dispatchEvent(const InputEvent& event);
    void update();

private:
    std::unordered_map<InputEventType, std::vector<InputCallback>> listeners;

    ScreenSpacePos currentMousePos{0.0f, 0.0f};
};