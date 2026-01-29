#version 460 core

in vec2 v_UV;

uniform sampler2D u_Texture; // just the currently bound texture

out vec4 FragColor;

void main() {
    FragColor = texture(u_Texture, v_UV);
}
