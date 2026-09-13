#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Common/Coordinates/WorldSpacePos.hpp"
#include "Common/Coordinates/ScreenSpacePos.hpp"

#include "Common/Math/Rect.hpp"
#include "Common/Debug/debug.hpp"

class Camera {
public:
    // zoom_distance is how far the camera sits from its target, along the
    // same fixed viewing direction the old hardcoded pos implied.
    explicit Camera(float zoom_distance = 4.58257569f);

    glm::mat4 getView() const;
    glm::mat4 getProjection(float aspectRatio) const;

    void moveCameraPos2D(glm::vec2 delta);
    void setCameraPos2D(WorldSpacePos position);
    void setPanBounds(Rect bounds) { camera_bounds = bounds; }

    // notches: positive = zoom in (closer to target), negative = zoom out.
    // Matches SDL's mouse wheel "y" delta directly (scroll up is positive).
    void zoom(float notches);
    float getZoomDistance() const { return glm::length(pos - target); }

    WorldSpacePos screenToWorldOnMap(
        ScreenSpacePos mousePixels,
        float screenWidth,
        float screenHeight
    ) const;

private:
    static WorldSpacePos fromRenderWorld(glm::vec3 position);
    static glm::vec3 toRenderWorld(WorldSpacePos position, float height = 0.0f);

    glm::vec3 pos;
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 0.0f, -1.0f);

    Rect camera_bounds {{-12,-12}, {12, 12}};
};