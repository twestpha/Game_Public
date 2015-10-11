#version 150

in vec2 Texcoord;

out vec4 outColor;

uniform bool is_outline;

void main() {
    if (is_outline){
        outColor = vec4(0.3, 0.3, 1.0, 1.0);
    } else{
        outColor = vec4(0.3, 0.3, 0.8, 0.4);
    }
}
