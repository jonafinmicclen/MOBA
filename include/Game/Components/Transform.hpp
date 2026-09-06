#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Common/Coordinates/FixedVec3.hpp"


struct Transform {
    FixedVec3 position {};
    glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale    {1.0f};

    glm::mat4 toMat4() const {
        glm::mat4 model(1.0f);
        model = glm::translate(model, position.toGlm());
        model *= glm::mat4_cast(rotation);
        model = glm::scale(model, scale);
        return model;
    }
};
