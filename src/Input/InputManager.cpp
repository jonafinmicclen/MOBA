#include "Input/InputManager.hpp"


void InputManager::dispatchEvent(InputEvent& event) {
    auto it = listeners.find(event.type);
    if (it != listeners.end()) {
        for (auto* listener : listeners[event.type]) {
            listener->OnInputEvent(event);
        }
    } else {
        DEBUG_LOG("Unhandled Event; Type: "<<(int)event.type);
    }

}

void InputManager::addListener(IInputListener* listener) {
    for (InputEventType interest : listener->getInterests()) {
        if (listeners.contains(interest)) {
            listeners[interest].push_back(listener);
        } else {
            listeners[interest] = {listener};
        }
    }
}

void InputManager::update() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        InputEvent event;
        switch (sdlEvent.type) {

            case SDL_KEYDOWN:
                if (!sdlEvent.key.repeat) {
                    event.type = InputEventType::KeyDown;
                    event.key = sdlEvent.key.keysym.sym;
                    dispatchEvent(event);
                }
                break;

            case SDL_KEYUP:
                event.type = InputEventType::KeyUp;
                event.key = sdlEvent.key.keysym.sym;
                dispatchEvent(event);
                break;

            case SDL_MOUSEBUTTONDOWN:
                event.type = InputEventType::MouseButtonDown;
                event.mousePos = {
                    static_cast<float>(sdlEvent.button.x),
                    static_cast<float>(sdlEvent.button.y)
                };
                event.mouseButton = sdlEvent.button.button;
                dispatchEvent(event);
                break;

            case SDL_MOUSEBUTTONUP:
                event.type = InputEventType::MouseButtonUp;
                event.mousePos = {
                    static_cast<float>(sdlEvent.button.x),
                    static_cast<float>(sdlEvent.button.y)
                };
                event.mouseButton = sdlEvent.button.button;
                dispatchEvent(event);
                break;

            case SDL_MOUSEMOTION:
                break;
                event.type = InputEventType::MouseMotion;
                event.mousePos = {
                    static_cast<float>(sdlEvent.button.x),
                    static_cast<float>(sdlEvent.button.y)
                };
                dispatchEvent(event);
                break;

            case SDL_QUIT:
                event.type = InputEventType::Exit;
                dispatchEvent(event);
                break;

        }
    }
}