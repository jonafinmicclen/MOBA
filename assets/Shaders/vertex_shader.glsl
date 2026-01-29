#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UV;
layout(location = 3) in int a_TexID; // <- remove flat here

out vec2 v_UV;
flat out int v_TexID; // flat here for fragment shader

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
    v_UV = a_UV;
    v_TexID = a_TexID; // pass to fragment shader
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}
