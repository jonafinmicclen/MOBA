#pragma once

#ifdef DEBUG

#include <external/glad.h>
#include <glm/glm.hpp>

#include "Assets/WalkableMapLoader.hpp"
#include "Common/Coordinates/TransformGrid.hpp"

// Debug-only visual: one semi-transparent purple quad per walkable grid
// cell, built with the same map_from_world transform pathfinding uses - so
// it doubles as a visual check that the walkable map is loaded and aligned
// correctly. Built once (walkable data is static at runtime); draw() takes
// a height (render-space Z - see Transform::toMat4, which has no axis swap)
// each frame so the caller can slide it to wherever the character currently
// stands without rebuilding the mesh.
class WalkableGridOverlay {
public:
    WalkableGridOverlay(const WalkableMap& walkable_area, const TransformGrid& map_from_world);
    ~WalkableGridOverlay();

    WalkableGridOverlay(const WalkableGridOverlay&) = delete;
    WalkableGridOverlay& operator=(const WalkableGridOverlay&) = delete;

    // scale multiplies the already-transformed world-space mesh about the
    // world origin (1.0 = no change). Lets the *rendered* grid size be
    // tuned by eye against the visible ground independently of rebuilding
    // the mesh - the corrected map_from_world.scale to put in map.json is
    // then (current json scale) / scale.
    void draw(const glm::mat4& view, const glm::mat4& proj, float height, float scale) const;

private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint ebo_ = 0;
    GLsizei index_count_ = 0;

    GLuint shader_program_ = 0;
    GLint u_model_ = -1;
    GLint u_view_ = -1;
    GLint u_proj_ = -1;
    GLint u_color_ = -1;
};

#endif
