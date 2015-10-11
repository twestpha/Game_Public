#version 150

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D base_texture;
uniform float opacity;

void main() {
    const int BLUR_RADIUS = 16;
    const int NUM_TEXELS = BLUR_RADIUS + 1;
    const float offsets[NUM_TEXELS] = float[](-15.5, -13.5, -11.5, -9.5, -7.5,
        -5.5, -3.5, -1.5, 0, 1.5, 3.5, 5.5, 7.5, 9.5, 11.5, 13.5, 15.5);

    vec4 texels[NUM_TEXELS];

    // Linear sample optimization from
    // http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
    texels[0] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[0]));
    texels[1] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[1]));
    texels[2] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[2]));
    texels[3] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[3]));
    texels[4] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[4]));
    texels[5] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[5]));
    texels[6] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[6]));
    texels[7] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[7]));
    texels[8] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[8]));
    texels[9] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[9]));
    texels[10] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[10]));
    texels[11] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[11]));
    texels[12] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[12]));
    texels[13] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[13]));
    texels[14] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[14]));
    texels[15] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[15]));
    texels[16] = textureOffset(base_texture, Texcoord, ivec2(0, offsets[16]));

    vec4 total = texels[0] + texels[1] + texels[2] + texels[3] + texels[4] +
        texels[5] + texels[6] + texels[7] + texels[8] +
        texels[9] + texels[10] + texels[11] + texels[12] +
        texels[13] + texels[14] + texels[15] + texels[16];

    vec4 average_texel = total / NUM_TEXELS;

    outColor = vec4(average_texel.rgb, opacity * average_texel.a);
}
