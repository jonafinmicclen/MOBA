#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Common/Coordinates/WorldSpacePos.hpp"
#include "Common/Coordinates/ScreenSpacePos.hpp"

class Camera {
public:
    glm::mat4 getView() const;
    glm::mat4 getProjection(float aspectRatio) const;

    void moveCameraPos2D(glm::vec2 delta);
    void setCameraPos2D(WorldSpacePos position);

    WorldSpacePos screenToWorldOnMap(
        ScreenSpacePos mousePixels,
        float screenWidth,
        float screenHeight
    ) const;

private:
    static WorldSpacePos fromRenderWorld(glm::vec3 position);
    static glm::vec3 toRenderWorld(WorldSpacePos position, float height = 0.0f);

private:
    glm::vec3 pos = glm::vec3(1.0f, 2.0f, -4.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 0.0f, -1.0f);
};