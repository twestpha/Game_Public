#version 150

in vec2 Texcoord;

out vec4 outColor;

uniform float opacity;
uniform sampler2D base_texture;

void main() {
    vec4 texel = textureOffset(base_texture, Texcoord, ivec2(-0.5 , -0.5));

    outColor = vec4(texel.rgb, opacity * texel.a);
}
