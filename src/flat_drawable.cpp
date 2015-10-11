#include "flat_drawable.hpp"

FlatDrawable::FlatDrawable(GLfloat width, GLfloat height, glm::vec2 position) : FlatDrawable(Shader("shaders/flat_drawable.vs",
    "shaders/flat_drawable.fs"), width, height, position){

}

FlatDrawable::FlatDrawable(Shader shader, GLfloat width, GLfloat height, glm::vec2 position) {

    load(shader.getGLId(), width, height, position);
}

void FlatDrawable::load(Shader shader, GLfloat width, GLfloat height, glm::vec2 position) {

    this->mesh = FlatMesh::getInstance();
    this->width = width;
    this->height = height;
    this->position = position;

    opacity = 1.0;
    setShader(shader.getGLId());
}

void FlatDrawable::draw(){
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glUseProgram(shader.getGLId());
    mesh->bindVAO();

    transformation = glm::mat3( width , 0     , position.x,
                                0     , height, position.y,
                                0     , 0     , 1           );

    glUniformMatrix3fv(glGetUniformLocation(shader.getGLId(), "transformation"), 1, GL_FALSE, glm::value_ptr(transformation));

    updateUniformData();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.getGLId());

    mesh->draw();
}

void FlatDrawable::updateUniformData(){
    glUniform1f(glGetUniformLocation(shader.getGLId(), "opacity"), opacity);
}

void FlatDrawable::attachTexture(Texture texture){
    glUseProgram(shader.getGLId());
    glUniform1i(glGetUniformLocation(shader.getGLId(), "base_texture"), 0);

    this->texture = texture;
}

void FlatDrawable::setOpacity(float opacity){
    this->opacity = opacity;
}

void FlatDrawable::setShader(Shader shader){
    this->shader = shader;
    mesh->attachGeometryToShader(shader);
}
