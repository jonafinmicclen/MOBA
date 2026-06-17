// WorldSpacePos.hpp
#pragma once


struct WorldSpacePos {
    float x{};
    float y{};

    WorldSpacePos operator-(const WorldSpacePos& other) const {
        return WorldSpacePos((x-other.x), (y-other.y));
    }

    WorldSpacePos operator/(const float& other) const {
        return {x/other, y/other};
    }

    WorldSpacePos operator*(const float& other) const {
        return {x*other, y*other};
    }
    
    float length() {
        return sqrtf(x*x + y*y);
    }
};