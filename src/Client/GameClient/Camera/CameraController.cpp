#include "Client/GameClient/Camera/CameraController.hpp"

#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>


void CameraController::update(const int window_width, const int window_height) {
    moveCameraTowardsMouse(window_width, window_height);
}

void CameraController::moveCameraTowardsMouse(const int window_width, const int window_height) {
    int x;
    int y;
    float dx = 0;
    float dy = 0;
    const float deadzone_proportion = 0.92f;
    SDL_GetMouseState(&x, &y);

    float x_from_centre = x - (float)window_width / 2;
    float y_from_centre = y - (float)window_height / 2;

    float x_deadzone = window_width*deadzone_proportion/2;
    float y_deadzone = window_height/2 - (window_width/2 - x_deadzone);

    if (abs(x_from_centre) > x_deadzone || abs(y_from_centre) > y_deadzone) {
        dx = x_from_centre / pan_speed_divisor_;
        dy = -y_from_centre / pan_speed_divisor_;
    }

    // dx/dy computed above mix direction and per-axis intensity together,
    // so in a corner (both large at once) the combined vector's length -
    // and therefore the actual pan speed - comes out up to sqrt(2)x faster
    // than hovering over a single edge. Separate the two: keep whichever
    // axis is more "pressed" as the speed, but always move along the true
    // normalized direction, so corners pan at the same speed as edges.
    float length = std::sqrt(dx * dx + dy * dy);
    if (length > 0.0f) {
        float speed = std::max(std::abs(dx), std::abs(dy));
        dx = (dx / length) * speed;
        dy = (dy / length) * speed;
    }

    camera->moveCameraPos2D({dx, dy});

}