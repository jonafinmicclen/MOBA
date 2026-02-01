#include "GameClient/Camera/Camera.hpp"


glm::mat4 Camera::getView() {
    return glm::lookAt(
        pos,
        target,
        up
    );
}

glm::mat4 Camera::getProjection(const float aspectRatio) {
    // Currently fixed, not good if i wanna change it but i dont think ill need to, at least for now
    return glm::perspective(
        glm::radians(37.0f),  // slightly narrower FOV
        aspectRatio,
        0.1f,
        100.0f
    );
}

void Camera::moveCameraPos2D(const glm::vec2 delta) {
    // Moves camera orthoganally to Up
    glm::vec3 forward = glm::normalize(target - pos);
    glm::vec3 right   = glm::normalize(glm::cross(forward, up));

    glm::vec3 groundForward = glm::normalize(
        glm::vec3(forward.x, forward.y, 0.0f)
    );

    glm::vec3 move =
        right * delta.x +
        groundForward * delta.y;

    pos    += move;
    target += move;
}

void Camera::setCameraPos2D(const WorldSpacePos position) {
    auto posToTarget = target - pos;
    pos.x = position.x;
    pos.y = position.y;
    target = pos + posToTarget;
}