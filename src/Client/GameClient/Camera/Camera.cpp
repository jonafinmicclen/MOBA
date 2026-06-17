#include "Client/GameClient/Camera/Camera.hpp"

#include <cmath>

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

    // Hardcoded XY camera-position boundary.
    newPos.x = glm::clamp(newPos.x, -3.0f, 3.0f);
    newPos.y = glm::clamp(newPos.y, -3.0f, 3.0f);

    // Apply only the movement that survived the clamp.
    glm::vec3 actualMove = newPos - pos;

    pos    += actualMove;
    target += actualMove;

}

void Camera::setCameraPos2D(const WorldSpacePos position) {
    auto posToTarget = target - pos;
    pos.x = position.x;
    pos.y = position.y;
    target = pos + posToTarget;
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
        position.x,
        height,
        position.y
    };
}