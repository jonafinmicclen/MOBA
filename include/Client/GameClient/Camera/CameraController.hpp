#pragma once

#include "Client/GameClient/Camera/Camera.hpp"
#include "Common/Math/Rect.hpp"

#include "Common/Debug/debug.hpp"
#include "Client/Input/InputManager.hpp"

// The client's camera controller: polls the mouse each frame to pan
// (update()), and reacts to scroll-wheel events to zoom.
class CameraController {
    public:
    CameraController(Camera* cam, float pan_speed_divisor, InputManager& input_manager)
        : camera(cam), pan_speed_divisor_(pan_speed_divisor) {
        input_manager.addListener(InputEventType::MouseWheel, [this](const InputEvent& e) {
            camera->zoom(e.scrollDelta);
        });
    }
    void update(const int window_width, const int window_height);

    float getPanSpeedDivisor() const { return pan_speed_divisor_; }
    void setPanSpeedDivisor(float divisor) { pan_speed_divisor_ = divisor; }

    private:
    void moveCameraTowardsMouse(const int window_width, const int window_height);
    Camera* camera;
    float pan_speed_divisor_;

};