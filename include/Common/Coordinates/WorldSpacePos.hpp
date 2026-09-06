// WorldSpacePos.hpp
#pragma once

#include <cmath>

#include "Common/Math/Fixed.hpp"


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
};
