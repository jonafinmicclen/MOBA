#include "Client/GameClient/Camera/Camera.hpp"

#include <cmath>

Camera::Camera(float zoom_distance) {
    // Same direction the old hardcoded (1,2,-4) pos implied, just now with
    // a configurable distance from target instead of a fixed magnitude.
    glm::vec3 direction = glm::normalize(glm::vec3(1.0f, 2.0f, -4.0f));
    pos = direction * zoom_distance;
}

void Camera::zoom(float notches) {
    constexpr float kStepPerNotch = 0.5f;
    constexpr float kMinDistance = 1.0f;
    constexpr float kMaxDistance = 30.0f;

    glm::vec3 offset = pos - target;
    float distance = glm::length(offset);
    if (distance < 0.0001f) {
        return; // degenerate - pos and target coincide, no direction to preserve
    }
    glm::vec3 direction = offset / distance;

    float newDistance = glm::clamp(distance - notches * kStepPerNotch, kMinDistance, kMaxDistance);
    pos = target + direction * newDistance;
}

glm::mat4 Camera::getView() const {
    return glm::lookAt(
        pos,
        target,
        up
    );
}

glm::mat4 Camera::getProjection(float aspectRatio) const {
    return glm::perspective(
        glm::radians(37.0f),
        aspectRatio,
        0.1f,
        100.0f
    );
}

void Camera::moveCameraPos2D(const glm::vec2 delta)
{
    glm::vec3 forward = glm::normalize(target - pos);
    glm::vec3 right   = glm::normalize(glm::cross(forward, up));

    glm::vec3 groundForward = glm::normalize(
        glm::vec3(forward.x, forward.y, 0.0f)
    );

    glm::vec3 move =
        right * delta.x +
        groundForward * delta.y;

    glm::vec3 newPos = pos + move;

    newPos.x = glm::clamp(newPos.x, camera_bounds.min_corner.x.toFloat(), camera_bounds.max_corner.x.toFloat());
    newPos.y = glm::clamp(newPos.y, camera_bounds.min_corner.y.toFloat(), camera_bounds.max_corner.y.toFloat());

    // Apply only the movement that survived the clamp.
    glm::vec3 actualMove = newPos - pos;

    pos    += actualMove;
    target += actualMove;

}

void Camera::setCameraPos2D(const WorldSpacePos position) {
    // It's target (the look-at point) that needs to land on `position` for
    // it to appear centered on screen, not pos (the camera's eye) - shift
    // both together by the same delta, same as moveCameraPos2D's panning,
    // so the camera's fixed viewing angle/distance is preserved.
    float dx = position.x.toFloat() - target.x;
    float dy = position.y.toFloat() - target.y;

    pos.x += dx;
    pos.y += dy;
    target.x += dx;
    target.y += dy;
}

WorldSpacePos Camera::screenToWorldOnMap(
    ScreenSpacePos mousePixels,
    float screenWidth,
    float screenHeight
) const {
    float aspectRatio = screenWidth / screenHeight;

    float ndcX = (2.0f * mousePixels.x) / screenWidth - 1.0f;
    float ndcY = 1.0f - (2.0f * mousePixels.y) / screenHeight;

    glm::mat4 view = getView();
    glm::mat4 projection = getProjection(aspectRatio);
    glm::mat4 invViewProjection = glm::inverse(projection * view);

    glm::vec4 nearClip{ndcX, ndcY, -1.0f, 1.0f};
    glm::vec4 farClip{ndcX, ndcY, 1.0f, 1.0f};

    glm::vec4 nearWorld4 = invViewProjection * nearClip;
    glm::vec4 farWorld4 = invViewProjection * farClip;

    nearWorld4 /= nearWorld4.w;
    farWorld4 /= farWorld4.w;

    glm::vec3 rayOrigin = glm::vec3(nearWorld4);
    glm::vec3 rayEnd = glm::vec3(farWorld4);
    glm::vec3 rayDir = glm::normalize(rayEnd - rayOrigin);

    // Your old camera movement implies the map plane is z = 0.
    constexpr float GroundZ = 0.0f;

    if (std::abs(rayDir.z) < 0.00001f) {
        return WorldSpacePos{target.x, target.y};
    }

    float t = (GroundZ - rayOrigin.z) / rayDir.z;

    if (t < 0.0f) {
        return WorldSpacePos{target.x, target.y};
    }

    glm::vec3 hit = rayOrigin + rayDir * t;

    return WorldSpacePos{
        hit.x,
        hit.y
    };
}

WorldSpacePos Camera::fromRenderWorld(glm::vec3 position) {
    // render/world x -> gameplay x
    // render/world z -> gameplay y
    return WorldSpacePos{
        position.x,
        position.z
    };
}

glm::vec3 Camera::toRenderWorld(WorldSpacePos position, float height) {
    // gameplay x -> render/world x
    // gameplay y -> render/world z
    // height     -> render/world y
    return glm::vec3{
        position.x.toFloat(),
        height,
        position.y.toFloat()
    };
}