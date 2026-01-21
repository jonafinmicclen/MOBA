#version 460 core

layout(location = 0) in vec3 aPos;    // vertex position
layout(location = 1) in vec3 aNormal; // vertex normal (for lighting later)
layout(location = 2) in vec2 aUV;     // texture coordinates

uniform mat4 u_Model;       // per-mesh transform
uniform mat4 u_View;        // camera/view transform
uniform mat4 u_Projection;  // projection matrix

out vec2 TexCoords;         // pass UVs to fragment shader
out vec3 FragNormal;        // pass normal to fragment shader (optional, for lighting)
out vec3 FragPos;           // vertex world position (optional, for lighting)

void main()
{
    // Transform vertex to world space
    vec4 worldPos = u_Model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;

    // Transform normal properly
    FragNormal = mat3(transpose(inverse(u_Model))) * aNormal;

    // Pass UVs
    TexCoords = aUV;

    // Transform to clip space
    gl_Position = u_Projection * u_View * worldPos;
}
