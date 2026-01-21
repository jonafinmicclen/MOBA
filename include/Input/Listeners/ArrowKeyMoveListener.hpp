#pragma once

#include "Input/IInputListener.hpp"
#include "Input/InputEventTypes.hpp"

#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

class ArrowKeyMoveListener : public IInputListener {
    public:
    glm::vec3* translation;

    ArrowKeyMoveListener(glm::vec3* translationF) : translation(translationF) {}

    void OnInputEvent(InputEvent& event) override {
        switch (event.key) {
            case SDLK_UP:
                translation->x -= 0.1f;
            case SDLK_DOWN:
                translation-> x +=0.1f;
            case SDLK_LEFT:
                translation->y -= 0.1f;
            case SDLK_RIGHT:
                translation->y += 0.1f;
        }
    }

    std::vector<InputEventType> getInterests() override {
        return { InputEventType::KeyDown };
    }

};