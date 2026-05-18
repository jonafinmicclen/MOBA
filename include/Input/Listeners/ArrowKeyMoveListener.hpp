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
                translation->x -= 1.0f;
                break;
            case SDLK_DOWN:
                translation-> x +=1.0f;
                break;
            case SDLK_LEFT:
                translation->z -= 1.0f;
                break;
            case SDLK_RIGHT:
                translation->z += 1.0f;
                break;
        }
    }

    std::vector<InputEventType> getInterests() override {
        return { InputEventType::KeyDown };
    }

};