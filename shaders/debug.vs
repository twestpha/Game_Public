#version 150

in vec2 position;
in vec2 texcoord;

out vec2 Texcoord;

uniform mat3 transformation;

void main() {
    Texcoord = texcoord;
    vec3 position_temp = vec3(position, 1.0) * transformation;
    gl_Position = vec4(position_temp.xy, 0.0, 1.0);
}