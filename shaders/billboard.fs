#version 330

in vec2 Texcoord;

out vec4 outColor;

uniform float opacity;
uniform sampler2D emissive_texture;

void main() {
    vec4 texel = texture(emissive_texture, Texcoord);
    outColor = vec4(texel.rgb, opacity * texel.a);
}
