#pragma once

#include "Input/IInputListener.hpp"

class ExitListener : public IInputListener {
    
public:
    bool* running;
    ExitListener(bool* runningFlag) : running(runningFlag) {}

    void OnInputEvent(InputEvent& event) override {
        *running = false;
    }

    std::vector<InputEventType> getInterests() override {
        return { InputEventType::Exit };
    }
};