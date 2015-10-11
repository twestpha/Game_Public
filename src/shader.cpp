#include "shader.hpp"

Shader::Shader(){
    gl_shader_id = 0;
}

Shader::Shader(GLuint shader){
    gl_shader_id = shader;
}

Shader::Shader(string vs_filename, string fs_filename) {
    gl_shader_id = loadShaderProgram(vs_filename, fs_filename);
}

GLuint Shader::getGLId() {
    return gl_shader_id;
}

GLuint Shader::loadVertexShader(string vs_filename){
    // Create the vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

    ifstream vs_input(vs_filename);
    string vs_source((istreambuf_iterator<char>(vs_input)), istreambuf_iterator<char>());
    const char* vs_source_c = vs_source.c_str();

    glShaderSource(vertex_shader, 1, &vs_source_c, NULL);

    // Compile it
    glCompileShader(vertex_shader);

    GLint status;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE){
        char info_log[512];
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        Debug::error("Failed to compile the vertex shader '%s':\n%s\n", vs_filename.c_str(),
            info_log);
    }

    return vertex_shader;
}

GLuint Shader::loadFragmentShader(string fs_filename){
    // Create the fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    ifstream fs_input(fs_filename);
    string fs_source((istreambuf_iterator<char>(fs_input)), istreambuf_iterator<char>());
    const char* fs_source_c = fs_source.c_str();

    glShaderSource(fragment_shader, 1, &fs_source_c, NULL);

    // Compile it
    glCompileShader(fragment_shader);

    GLint status;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE){
        char info_log[512];
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        Debug::error("Failed to compile the fragment shader '%s':\n%s\n", fs_filename.c_str(),
            info_log);
    }

    return fragment_shader;
}

GLuint Shader::combineShaderProgram(GLuint vertex_shader, GLuint fragment_shader) {
    // Combine the shaders into a single program
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    // Put the shader on the graphics card.
    glLinkProgram(shader_program);

    // Tell the fragment shader to use something??
    glBindFragDataLocation(shader_program, 0, "outColor");

    // Free up the space from the vetex and fragment shader because
    // they have been combined into the shader program.
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}

GLuint Shader::loadShaderProgram(string vs_filename, string fs_filename) {
    GLuint shader_program;

    GLuint vertex_shader = loadVertexShader(vs_filename);
    GLuint fragment_shader = loadFragmentShader(fs_filename);
    shader_program = combineShaderProgram(vertex_shader, fragment_shader);

    return shader_program;
}
