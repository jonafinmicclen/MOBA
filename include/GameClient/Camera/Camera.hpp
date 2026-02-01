#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "Common/Coordinates/WorldSpacePos.hpp"


class Camera {
    public:
    glm::mat4 getView();
    glm::mat4 getProjection(const float aspectRatio);

    void moveCameraPos2D(const glm::vec2 delta);
    void setCameraPos2D(const WorldSpacePos position);

    private:
    glm::vec3 pos = glm::vec3(1.0f, 2.0f, -4.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f,  0.0f);
    glm::vec3 up = glm::vec3(0.0f, 0.0f, -1.0f);
};