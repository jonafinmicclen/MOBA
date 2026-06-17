#pragma once

#include "GameClient/Camera/Camera.hpp"
#include "Common/Math/Rect.hpp"

#include "Common/Debug/debug.hpp"

class CameraController {
    public:
    CameraController(Camera* cam) : camera(cam) {}
    void update(const int window_width, const int window_height);
    private:
    void moveCameraTowardsMouse(const int window_width, const int window_height);
    Camera* camera;
    
};