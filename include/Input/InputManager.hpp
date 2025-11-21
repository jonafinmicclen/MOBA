// InputManager.hpp
#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <unordered_map>

#include "Input/InputEventTypes.hpp"
#include "Input/IInputListener.hpp"


class InputManager {
    private:
    std::unordered_map<InputEventType, std::vector<IInputListener*>> listeners;
    void DispatchEvent(InputEvent& event);

    
    public:
    void AddListener(IInputListener* listener);
    void Update();
};