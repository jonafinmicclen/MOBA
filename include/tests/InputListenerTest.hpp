#pragma once

#include <iostream>
#include "Input/InputManager.hpp"
#include "Input/IInputListener.hpp"
#include "Renderer/Renderer.hpp"

class TestListener : public IInputListener {
public:
    void OnInputEvent(InputEvent& event) override {
        std::cout<<"Input recieved"<<std::endl;
    }

    std::vector<InputEventType> getInterests() override {
        return {InputEventType::KeyUp, InputEventType::MouseButtonUp};
    }
};

class ExitListener : public IInputListener {
public:
    bool shouldExit = false;

    void OnInputEvent(InputEvent& event) override {
        if (event.type == InputEventType::Exit) {
            shouldExit = true;
        }
    }

    std::vector<InputEventType> getInterests() override {
        return {InputEventType::Exit};
    }
};

class InputListenerTest {
public:
    void echoInputs() {
        auto renderer = Renderer();
        auto manager = InputManager();
        auto useless = TestListener();
        auto listener = TestListener();
        auto exitListener = ExitListener();
        manager.AddListener(&listener);
        manager.AddListener(&exitListener);
        manager.AddListener(&useless);

        while(!exitListener.shouldExit) {
            manager.Update();
        }
    }
};