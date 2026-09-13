#pragma once

#include "Common/Math/Fixed.hpp"
#include "Common/Coordinates/Vec2.hpp"

enum class Rotation90 : uint8_t { Deg0, Deg90, Deg180, Deg270 };

struct TransformGrid {
    Vec2 offset;
    Vec2 scale;
    Rotation90 rotation = Rotation90::Deg0;

    Vec2 operator*(const Vec2& v) const {
        // 1. Scale
        Vec2 scaled { v.x * scale.x, v.y * scale.y };

        // 2. Rotate (exact - lookup, no trig)
        Vec2 rotated;
        switch (rotation) {
            case Rotation90::Deg0:   rotated = { scaled.x, scaled.y };  break;
            case Rotation90::Deg90:  rotated = { -scaled.y, scaled.x }; break;
            case Rotation90::Deg180: rotated = { -scaled.x, -scaled.y }; break;
            case Rotation90::Deg270: rotated = { scaled.y, -scaled.x }; break;
        }

        // 3. Translate
        return Vec2 { rotated.x + offset.x, rotated.y + offset.y };
    }

    // Undoes operator* : translate back, un-rotate, then un-scale.
    // Order matters here, so this isn't just "build a TransformGrid with
    // reciprocal fields and reuse operator*" - that would apply the steps
    // in the wrong order.
    [[nodiscard]] Vec2 applyInverse(const Vec2& v) const {
        Vec2 untranslated { v.x - offset.x, v.y - offset.y };

        Vec2 unrotated;
        switch (rotation) {
            case Rotation90::Deg0:   unrotated = untranslated; break;
            case Rotation90::Deg90:  unrotated = { untranslated.y, -untranslated.x }; break;
            case Rotation90::Deg180: unrotated = { -untranslated.x, -untranslated.y }; break;
            case Rotation90::Deg270: unrotated = { -untranslated.y, untranslated.x }; break;
        }

        return Vec2 { unrotated.x / scale.x, unrotated.y / scale.y };
    }
};