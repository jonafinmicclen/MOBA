#pragma once

#include "Common/Debug/DebugOverlay.hpp"

// Thin system wrapper around DebugOverlay so Client and Server can each hold
// and update it the same way as any other per-frame/tick system, instead of
// hand-rolling a DEBUG_STAT_FLUSH() call in both loops separately.
//
// Safe to instantiate and call update() on in Release builds too -
// DEBUG_STAT_FLUSH() is a no-op macro outside DEBUG builds, so this
// optimises away to nothing there.
class DebugOverlaySystem {
public:
    void update() {
        DEBUG_STAT_FLUSH();
    }
};
