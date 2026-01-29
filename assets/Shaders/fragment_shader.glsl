#version 460 core

in vec2 v_UV;
flat in int v_TexID; // flat in, matches vertex shader

uniform sampler2D u_Textures[250]; // adjust number as needed

out vec4 FragColor;

void main() {
    FragColor = texture(u_Textures[v_TexID], v_UV);
}
