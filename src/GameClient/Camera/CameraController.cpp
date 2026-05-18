#include "GameClient/Camera/CameraController.hpp"

#include <SDL2/SDL.h>


void CameraController::update(const int window_width, const int window_height) {
    moveCameraTowardsMouse(window_width, window_height);
}

void CameraController::moveCameraTowardsMouse(const int window_width, const int window_height) {
    int x;
    int y;
    float dx = 0;
    float dy = 0;
    const float deadzone_proportion = 0.92f;
    const float inverse_pan_speed = 13000.0f;
    SDL_GetMouseState(&x, &y);

    float x_from_centre = x - (float)window_width / 2;
    float y_from_centre = y - (float)window_height / 2;

    float x_deadzone = window_width*deadzone_proportion/2;
    float y_deadzone = window_height/2 - (window_width/2 - x_deadzone);

    if (abs(x_from_centre) > x_deadzone || abs(y_from_centre) > y_deadzone) {
        dx += x_from_centre / inverse_pan_speed;
        dy -= y_from_centre / inverse_pan_speed;
    }
    
    camera->moveCameraPos2D({dx, dy});

}