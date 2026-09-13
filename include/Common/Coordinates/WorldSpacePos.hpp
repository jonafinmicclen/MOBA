// WorldSpacePos.hpp
#pragma once

#include <cmath>

#include "Common/Math/Fixed.hpp"
#include "Common/Coordinates/Vec2.hpp"


struct WorldSpacePos {
    Fixed x{};
    Fixed y{};

    WorldSpacePos operator-(const WorldSpacePos& other) const {
        return WorldSpacePos{(x-other.x), (y-other.y)};
    }

    WorldSpacePos operator/(const float& other) const {
        return {x/other, y/other};
    }

    WorldSpacePos operator*(const float& other) const {
        return {x*other, y*other};
    }
    
    float length() {
        return sqrtf(x.toFloat()*x.toFloat() + y.toFloat()*y.toFloat());
    }

    [[nodiscard]] Vec2 toVec2() const {
        return Vec2{x, y};
    }

    [[nodiscard]] static WorldSpacePos fromVec2(const Vec2& v) {
        return WorldSpacePos{v.x, v.y};
    }
};
