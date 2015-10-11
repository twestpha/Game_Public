// aa.fs
// Trevor Westphal

#version 330

in vec2 Texcoord;

out vec4 outColor;

uniform sampler2D base_texture;

// Neighbor contrast tuning
float THRESHOLD = 0.9;
float MIXAMOUNT = 0.3;


float FxaaLuma(vec4 color) {
    // As an optimization, luminance is estimated strictly from Red and Green
    // channels using a single fused multiply add operation. In practice pure
    // blue aliasing rarely appears in typical game content.
    return color.g * (0.587/0.299) + color.r;
}

void main() {
    vec4 current_pixel = texture(base_texture, Texcoord);
    float current_luminance = FxaaLuma(current_pixel);

    // Fetch the pixels from the image
    vec4 n_pixel = textureOffset(base_texture, Texcoord, ivec2(0, 1));
    vec4 s_pixel = textureOffset(base_texture, Texcoord, ivec2(0, -1));
    vec4 e_pixel = textureOffset(base_texture, Texcoord, ivec2(1, 0));
    vec4 w_pixel = textureOffset(base_texture, Texcoord, ivec2(-1, 0));

    vec4 nw_pixel = textureOffset(base_texture, Texcoord, ivec2(-1,-1));
    vec4 ne_pixel = textureOffset(base_texture, Texcoord, ivec2( 1,-1));
    vec4 sw_pixel = textureOffset(base_texture, Texcoord, ivec2(-1, 1));
    vec4 se_pixel = textureOffset(base_texture, Texcoord, ivec2( 1, 1));

    // Get the luminosity between the edge pixels
    float r_diagonal_luma_diff = abs(FxaaLuma(ne_pixel) - FxaaLuma(sw_pixel));
    float l_diagonal_luma_diff = abs(FxaaLuma(nw_pixel) - FxaaLuma(se_pixel));
    float horizontal_luma_diff = abs(FxaaLuma(e_pixel) - FxaaLuma(w_pixel));
    float vertical_luma_diff   = abs(FxaaLuma(n_pixel) - FxaaLuma(s_pixel));

    // mark which pixels need aliasing
    bool alias_r_diagonal = (r_diagonal_luma_diff > THRESHOLD) && (r_diagonal_luma_diff < current_luminance);
    bool alias_l_diagonal = (l_diagonal_luma_diff > THRESHOLD) && (l_diagonal_luma_diff < current_luminance);
    bool alias_horizontal = (horizontal_luma_diff > THRESHOLD) && (horizontal_luma_diff < current_luminance);
    bool alias_vertical   = (vertical_luma_diff > THRESHOLD)   && (vertical_luma_diff   < current_luminance)  ;

    // Debugging edges
    if(alias_r_diagonal || alias_l_diagonal || alias_horizontal || alias_vertical){
        outColor = vec4(1.0, 0.0, 0.0, 1.0);
    } else {
        outColor = current_pixel;
    }

    // outColor = current_pixel;

    // vec4 color = current_pixel;
    //
    // if(alias_r_diagonal){
    //     if(FxaaLuma(ne_pixel) > FxaaLuma(sw_pixel)){
    //         color = mix(color, sw_pixel, MIXAMOUNT);
    //     } else {
    //         color = mix(color, ne_pixel, MIXAMOUNT);
    //     }
    // }
    //
    // if(alias_l_diagonal){
    //     if(FxaaLuma(nw_pixel) > FxaaLuma(se_pixel)){
    //         color = mix(color, se_pixel, MIXAMOUNT);
    //     } else {
    //         color = mix(color, nw_pixel, MIXAMOUNT);
    //     }
    // }
    //
    // if(alias_horizontal){
    //     if(FxaaLuma(e_pixel) > FxaaLuma(w_pixel)){
    //         color = mix(color, w_pixel, MIXAMOUNT);
    //     } else {
    //         color = mix(color, e_pixel, MIXAMOUNT);
    //     }
    // }
    //
    // if(alias_vertical){
    //     if(FxaaLuma(n_pixel) > FxaaLuma(s_pixel)){
    //         color = mix(color, s_pixel, MIXAMOUNT);
    //     } else {
    //         color = mix(color, n_pixel, MIXAMOUNT);
    //     }
    // }
    //
    // outColor = color;

}
