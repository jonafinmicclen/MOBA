#ifdef DEBUG

#include "Client/Debug/WalkableGridOverlay.hpp"
#include "Client/Renderer/Shader.hpp"
#include "Common/Coordinates/Vec2.hpp"
#include "Common/Math/Fixed.hpp"
#include "Common/Debug/debug.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <cstdint>
#include <vector>

namespace {

constexpr const char* kVertexShader = R"glsl(
#version 330 core
layout(location = 0) in vec3 a_Position;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}
)glsl";

constexpr const char* kFragmentShader = R"glsl(
#version 330 core
out vec4 FragColor;
uniform vec4 u_Color;

void main() {
    FragColor = u_Color;
}
)glsl";

} // namespace

WalkableGridOverlay::WalkableGridOverlay(const WalkableMap& walkable_area, const TransformGrid& map_from_world) {
    const int width = walkable_area.getWidth();
    const int height = walkable_area.getHeight();

    std::vector<glm::vec3> vertices;
    std::vector<uint32_t> indices;

    // Gameplay x -> render x, gameplay y -> render y (Transform::toMat4()
    // and PathFollowingSystem write straight into position.x/position.y
    // with no axis swap - render Z is the height/up axis, not Y). Height is
    // baked as 0 here along Z and applied via the model matrix at draw
    // time, so the mesh never needs rebuilding when the tracked height
    // changes.
    auto gridToRender = [&](int gx, int gy) {
        Vec2 grid { Fixed(static_cast<float>(gx)), Fixed(static_cast<float>(gy)) };
        Vec2 world = map_from_world.applyInverse(grid);
        return glm::vec3(world.x.toFloat(), world.y.toFloat(), 0.0f);
    };

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (!walkable_area.at(x, y)) continue;

            uint32_t base = static_cast<uint32_t>(vertices.size());
            vertices.push_back(gridToRender(x,     y));
            vertices.push_back(gridToRender(x + 1, y));
            vertices.push_back(gridToRender(x + 1, y + 1));
            vertices.push_back(gridToRender(x,     y + 1));

            // Wound so the normal (v1-v0) x (v2-v0) points +Z (up at the
            // camera) rather than down through the floor.
            indices.push_back(base + 0);
            indices.push_back(base + 1);
            indices.push_back(base + 2);
            indices.push_back(base + 0);
            indices.push_back(base + 2);
            indices.push_back(base + 3);
        }
    }

    index_count_ = static_cast<GLsizei>(indices.size());

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(glm::vec3)), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(uint32_t)), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    shader_program_ = createShaderProgram(kVertexShader, kFragmentShader);
    u_model_ = glGetUniformLocation(shader_program_, "u_Model");
    u_view_  = glGetUniformLocation(shader_program_, "u_View");
    u_proj_  = glGetUniformLocation(shader_program_, "u_Projection");
    u_color_ = glGetUniformLocation(shader_program_, "u_Color");

    DEBUG_LOG("WalkableGridOverlay built " << index_count_ / 6 << " walkable quads ("
        << index_count_ << " indices) out of " << width << "x" << height << " grid cells");
}

WalkableGridOverlay::~WalkableGridOverlay() {
    glDeleteBuffers(1, &ebo_);
    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);
    glDeleteProgram(shader_program_);
}

void WalkableGridOverlay::draw(const glm::mat4& view, const glm::mat4& proj, float height, float scale) const {
    if (index_count_ == 0) return;

    // Depth test off, not just depth write: the map mesh's actual ground
    // isn't a flat Z=0 plane the way this was first assumed, so no fixed
    // height offset reliably clears it everywhere. A debug overlay should
    // always be visible on top regardless of terrain anyway.
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shader_program_);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, height));
    model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
    glUniformMatrix4fv(u_model_, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(u_view_, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(u_proj_, 1, GL_FALSE, &proj[0][0]);
    glUniform4f(u_color_, 0.55f, 0.0f, 0.85f, 0.35f);

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, index_count_, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

#endif
