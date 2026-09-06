// FixedVec3.hpp
#pragma once

#include <glm/glm.hpp>

#include "Common/Math/Fixed.hpp"

// Fixed-point equivalent of glm::vec3, used for entity world positions
// (Transform::position). Only convert to glm::vec3 at the render boundary,
// via toGlm().
struct FixedVec3 {
    Fixed x{};
    Fixed y{};
    Fixed z{};

    [[nodiscard]] glm::vec3 toGlm() const {
        return glm::vec3(x.toFloat(), y.toFloat(), z.toFloat());
    }
};
