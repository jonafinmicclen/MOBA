#pragma once

#ifdef DEBUG

#include <algorithm>

#include "Client/GameClient/Camera/CameraController.hpp"
#include "Client/Input/InputManager.hpp"
#include "Common/Debug/DebugOverlay.hpp"

// Debug-only: adjusts CameraController's pan speed divisor live via -/=
// (~500 per press) so the right feel can be found by eye instead of
// edit-rebuild-relaunch each try. Kept as its own class, separate from the
// real CameraController, so this debug-only concern never leaks into the
// non-debug camera control path.
class DebugPanSpeedController {
public:
    // enabled: shared master dev-mode toggle (F12 in GameClient) - checked
    // before applying any adjustment so it's included in "toggle all dev
    // things" without this class needing to know how that toggle works.
    DebugPanSpeedController(CameraController& camera_controller, InputManager& input_manager, const bool& enabled)
        : camera_controller_(camera_controller), enabled_(enabled) {
        input_manager.addListener(InputEventType::KeyDown, [this](const InputEvent& e) {
            if (!enabled_) return;
            if (e.key == SDLK_MINUS) {
                adjust(-500.0f);
            } else if (e.key == SDLK_EQUALS) {
                adjust(500.0f);
            }
        });
    }

private:
    void adjust(float delta) {
        // Floor above 0 - pan_speed_divisor_ is a divisor (see
        // CameraController::moveCameraTowardsMouse), so 0 is a div-by-zero
        // and negative would invert pan direction unexpectedly.
        float new_value = std::max(100.0f, camera_controller_.getPanSpeedDivisor() + delta);
        camera_controller_.setPanSpeedDivisor(new_value);
        DEBUG_STAT("CameraController pan speed divisor", new_value);
    }

    CameraController& camera_controller_;
    const bool& enabled_;
};

#endif
