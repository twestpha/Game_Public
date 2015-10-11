#version 330 core

// Ouput data
out vec4 outColor;

void main(){
    // Not really needed, OpenGL does it anyway
    outColor = vec4(gl_FragCoord.z * vec3(1.0, 1.0, 1.0), 1.0);
}
