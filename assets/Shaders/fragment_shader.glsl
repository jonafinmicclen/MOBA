#version 460 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    vec4 texColor = texture(texture1, TexCoords);

    // fallback color if texture is black
    if(texColor.a == 0.0 || texColor.rgb == vec3(0.0))
        texColor = vec4(0.8, 0.3, 0.3, 1.0);

    FragColor = texColor;
}
