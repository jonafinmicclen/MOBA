#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 u_MVP;

out vec2 TexCoords; // must match fragment shader 'in'

void main()
{
    TexCoords = aUV; // pass UVs to fragment shader
    gl_Position = u_MVP * vec4(aPos, 1.0);
}
