#include "Input/InputManager.hpp"


void InputManager::DispatchEvent(InputEvent& event) {
    auto it = listeners.find(event.type);
    if (it != listeners.end()) {
        for (auto* listener : listeners[event.type]) {
            listener->OnInputEvent(event);
        }
    } else {
        std::cout<<"[InputManager]: Unhandled Event ):"<<std::endl;
    }

}

void InputManager::AddListener(IInputListener* listener) {
    for (InputEventType interest : listener->getInterests()) {
        if (listeners.contains(interest)) {
            listeners[interest].push_back(listener);
        } else {
            listeners[interest] = {listener};
        }
    }
}

void InputManager::Update() {
    SDL_Event sdlEvent;
    if (SDL_PollEvent(&sdlEvent)) {
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

            case SDL_QUIT:
                event.type = InputEventType::Exit;
                DispatchEvent(event);
                break;
        }
    }
}