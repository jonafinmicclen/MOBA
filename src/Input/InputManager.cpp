#include "Input/InputManager.hpp"

void InputManager::addListener(InputEventType type, InputCallback callback) {
    listeners[type].push_back(std::move(callback));
}

void InputManager::addListener(std::initializer_list<InputEventType> types, InputCallback callback) {
    for (InputEventType type : types) {
        listeners[type].push_back(callback);
    }
}

void InputManager::dispatchEvent(const InputEvent& event) {
    auto it = listeners.find(event.type);

    if (it == listeners.end()) {
        DEBUG_LOG("Unhandled Event; Type: " << static_cast<int>(event.type));
        return;
    }

    for (const auto& callback : it->second) {
        callback(event);
    }
}

void InputManager::update() {
    SDL_Event sdlEvent;

    while (SDL_PollEvent(&sdlEvent)) {
        InputEvent event{};

        // Every event starts with the latest known mouse position.
        event.mousePos = currentMousePos;

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
                currentMousePos = {
                    static_cast<float>(sdlEvent.button.x),
                    static_cast<float>(sdlEvent.button.y)
                };

                event.type = InputEventType::MouseButtonDown;
                event.mousePos = currentMousePos;
                event.mouseButton = sdlEvent.button.button;
                dispatchEvent(event);
                break;

            case SDL_MOUSEBUTTONUP:
                currentMousePos = {
                    static_cast<float>(sdlEvent.button.x),
                    static_cast<float>(sdlEvent.button.y)
                };

                event.type = InputEventType::MouseButtonUp;
                event.mousePos = currentMousePos;
                event.mouseButton = sdlEvent.button.button;
                dispatchEvent(event);
                break;

            case SDL_MOUSEMOTION:
                currentMousePos = {
                    static_cast<float>(sdlEvent.motion.x),
                    static_cast<float>(sdlEvent.motion.y)
                };

                event.type = InputEventType::MouseMotion;
                event.mousePos = currentMousePos;
                dispatchEvent(event);
                break;

            case SDL_QUIT:
                event.type = InputEventType::Exit;
                event.mousePos = currentMousePos;
                dispatchEvent(event);
                break;

            default:
                break;
        }
    }
}