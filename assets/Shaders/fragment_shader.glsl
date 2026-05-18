#version 460 core

in vec2 v_UV;

uniform sampler2D u_Texture;

out vec4 FragColor;

void main() {
    vec4 tex = texture(u_Texture, v_UV);

    // Alpha mask (cutout)
    if (tex.a < 0.5)
        discard;

    // Output fully opaque pixel (cutout style)
    FragColor = vec4(tex.rgb, 1.0);
}
