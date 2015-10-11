#version 330 core

// Input vertex data, different for all executions of this shader.
layout (location=1) in vec3 position;
layout (location=2) in vec3 normal;
layout (location=3) in vec2 texcoord;

// Values that stay constant for the whole mesh.

layout(std140) uniform ShadowMatrices {
    mat4 depth_view;
    mat4 depth_proj;
};

uniform mat4 model;
uniform float scale;

void main(){
    gl_Position =  depth_proj * depth_view * model * vec4(scale * position, 1.0);
}
