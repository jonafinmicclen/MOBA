#include "Input/InputManager.hpp"


void InputManager::DispatchEvent(InputEvent& event) {
    for (auto listener : listeners) {
        listener->OnInputEvent(event);
    }
}

void InputManager::AddListener(IInputListener* listener) {
    listeners.push_back(listener);
}

void InputManager::Update() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        InputEvent event;
        switch (sdlEvent.type) {

            case SDL_KEYDOWN:
                if (!sdlEvent.key.repeat) {
                    event.type = InputEventType::KeyDown;
                    event.key = sdlEvent.key.keysym.sym;
                    DispatchEvent(event);
                }
                break;

            case SDL_KEYUP:
                event.type = InputEventType::KeyUp;
                event.key = sdlEvent.key.keysym.sym;
                DispatchEvent(event);
                break;

            case SDL_MOUSEBUTTONDOWN:
                event.type = InputEventType::MouseButtonDown;
                event.mousePos = {sdlEvent.button.x, sdlEvent.button.y};
                event.mouseButton = sdlEvent.button.button;
                DispatchEvent(event);
                break;

            case SDL_MOUSEBUTTONUP:
                event.type = InputEventType::MouseButtonUp;
                event.mousePos = {sdlEvent.button.x, sdlEvent.button.y};
                event.mouseButton = sdlEvent.button.button;
                DispatchEvent(event);
                break;
        }
    }
}