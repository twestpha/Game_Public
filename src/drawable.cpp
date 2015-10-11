// Drawable:
//      The Drawable class is a parent class for any object that needs to be rendered in our 3D world.
//      It has a position, rotation (local), a shader, and a pointer to a Mesh (geometry data).

#include "drawable.hpp"

Drawable::Drawable(Mesh& mesh, Shader& shader){
    load(mesh, shader, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
}

Drawable::Drawable(Mesh& mesh, Shader& shader, glm::vec3 position, GLfloat scale) {

    load(mesh, shader, position, scale);
}

void Drawable::load(Mesh& mesh, Shader& shader, glm::vec3 position, GLfloat scale) {
    // Set the position, rotation, scale, and mesh pointer
    this->position = position;
    this->mesh = &mesh;
    this->scale = scale;

    # warning here
    // // Setup the drawable such that if no textures
    // // are attached later, then it will be bright pink
    this->diffuse = Texture(glm::vec4(0, 0, 0, 0));
    this->specular = Texture(glm::vec4(0, 0, 0, 0));
    this->emissive = Texture(glm::vec4(0, 0, 0, 0));
    this->normal = Texture(glm::vec4(0.5, 0.5, 1, 1));
    this->gloss = Texture(glm::vec4(0, 0, 0, 0));

    // Set the shader program and load the geometry data
    // from the mesh onto it.
    setShader(shader);

}

void Drawable::setScale(GLfloat scale) {
    if (scale < 0) {
        scale = 0;
    }

    this->scale = scale;
}

void Drawable::setShader(Shader& shader_ref){
    this->shader = &shader_ref;
    this->mesh->attachGeometryToShader(*shader);

    #warning Global uniform bindings should only ever be called once for each shader
    GLint global_matrix_location = glGetUniformBlockIndex(shader->getGLId(), "GlobalMatrices");
    glUniformBlockBinding(shader->getGLId(), global_matrix_location, 1);

    GLint shadow_matrix_location = glGetUniformBlockIndex(shader->getGLId(), "ShadowMatrices");
    glUniformBlockBinding(shader->getGLId(), shadow_matrix_location, 2);

    GLint mouse_point_location = glGetUniformBlockIndex(shader->getGLId(), "Mouse");
    glUniformBlockBinding(shader->getGLId(), mouse_point_location, 3);

    GLint unit_data_location = glGetUniformBlockIndex(shader->getGLId(), "UnitData");
    glUniformBlockBinding(shader->getGLId(), unit_data_location, 10);

    GLint settings_location = glGetUniformBlockIndex(shader->getGLId(), "ProfileSettings");
    // Debug::info("settings_location = %d\n", settings_location);
    glUniformBlockBinding(shader->getGLId(), settings_location, 4);

    setTextureLocations();

}

void Drawable::setDiffuse(Texture diffuse) {
    # warning here
    // if (diffuse != 0){
    //     if (emissive == pink){
    //         // If this diffuse is being set right after instantiation
    //         // then the pink emissive would still override. So if the
    //         // emissive is pink, set it to alpha.
    //         this->emissive = Texture(glm::vec4(0, 0, 0, 1));
    //     }
        this->diffuse = diffuse;
    // } else {
    //     this->diffuse = Texture(glm::vec4(0, 0, 0, 1));
    // }
}

void Drawable::setSpecular(Texture specular) {
    # warning here
    // if (specular != 0){
        this->specular = specular;
    // } else {
    //     this->specular = Texture(glm::vec4(0, 0, 0, 1));
    // }
}
void Drawable::setEmissive(Texture emissive) {
    # warning here
    // if (emissive != 0){
        this->emissive = emissive;
    // } else if (diffuse == alpha) {
    //     this->emissive = Texture(glm::vec4(1, 0, 1, 1));
    // } else {
    //     this->emissive = Texture(glm::vec4(0, 0, 0, 1));
    // }
}
void Drawable::setNormal(Texture normal) {
    # warning here
    // if (normal != 0){
        this->normal = normal;
    // } else {
    //     this->normal = Texture(glm::vec4(0.5, 0.5, 1, 1));
    // }
}


void Drawable::bindTextures(){
    // Put each texture into the correct location for this Drawable. GL_TEXTURE0-3
    // correspond to the uniforms set in attachTextureSet(). This is where we actually tell the graphics card which textures to use.
    #warning make these functions in texture??
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse.getGLId());

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specular.getGLId());

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, emissive.getGLId());

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, normal.getGLId());

}

void Drawable::setTextureLocations(){
    // Try to set the texture locations
    glUniform1i(glGetUniformLocation(shader->getGLId(), "diffuse_texture"), 0);
    glUniform1i(glGetUniformLocation(shader->getGLId(), "specular_texture"), 1);
    glUniform1i(glGetUniformLocation(shader->getGLId(), "emissive_texture"), 2);
    glUniform1i(glGetUniformLocation(shader->getGLId(), "normal_map"), 3);
    glUniform1i(glGetUniformLocation(shader->getGLId(), "shadow_map"), 4);
}

void Drawable::draw(){
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glUseProgram(shader->getGLId());

    // Bind the Mesh's VAO. This lets us put transformations and textures on top of the geometry.
    mesh->bindVAO();

    // We need to update the model matrix to account for any rotations or translations that have occured since the last draw call.
    updateModelMatrix();

    // Update the current model, view, and projection matrices in the shader. These are standard for all Drawables so they should always be updated in draw. Child specific data is updated in updateUniformData().
    glUniformMatrix4fv(glGetUniformLocation(shader->getGLId(), "model"), 1, GL_FALSE, glm::value_ptr(model_matrix));

    // Update other shader data
    updateUniformData();

    // Make sure to use this Drawable's textures
    bindTextures();

    // Draw the geometry
    mesh->draw();
}

void Drawable::setRotationEuler(GLfloat x, GLfloat y, GLfloat z){
    // Clear the rotation matrix to the identity matrix
    rotation_matrix = glm::mat4();

    // Clear the rotation vector
    rotation = glm::vec3(0.0f, 0.0f, 0.0f);

    // Rotate the specified amount.
    rotateGlobalEuler(x, y, z);
}

void Drawable::setRotationEuler(glm::vec3 rotation){
    Drawable::setRotationEuler(rotation.x, rotation.y, rotation.z);
}

void Drawable::rotateGlobalEuler(GLfloat x, GLfloat y, GLfloat z){

    // update the rotation vector
    rotation = rotation + glm::vec3(x, y, z);
    // Rotate the model about each axis.
    float cx = cos(x);
    float sx = sin(x);

    float cy = cos(y);
    float sy = sin(y);

    float cz = cos(z);
    float sz = sin(z);

    glm::mat4 rotation_z = glm::mat4( cz, -sz, 0, 0,
                                      sz,  cz, 0, 0,
                                      0 ,  0 , 1, 0,
                                      0 ,  0 , 0, 1);

    glm::mat4 rotation_x = glm::mat4( 1, 0 ,  0 , 0,
                                      0, cx, -sx, 0,
                                      0, sx,  cx, 0,
                                      0, 0 ,  0 , 1);

    glm::mat4 rotation_y = glm::mat4(  cy,  0, -sy, 0,
                                       0 ,  1,  0 , 0,
                                       sy,  0,  cy, 0,
                                       0 ,  0,  0 , 1);

    // The convention followed is rotate around X-axis, then Y-axis, and finally Z-axis.
    rotation_matrix = rotation_x * rotation_matrix;
    rotation_matrix = rotation_y * rotation_matrix;
    rotation_matrix = rotation_z * rotation_matrix;
}

void Drawable::rotateGlobalEuler(glm::vec3 rotation){
    Drawable::rotateGlobalEuler(rotation.x, rotation.y, rotation.z);
}

void Drawable::rotateAxisAngle(glm::vec3 axis, GLfloat angle){
    glm::quat quaternion =  glm::angleAxis(angle, axis);
    rotation_matrix = glm::toMat4(quaternion);

}

void Drawable::setPosition(glm::vec3 position) {
    this->position = position;
}
void Drawable::setPosition(GLfloat x, GLfloat y, GLfloat z) {
    position = glm::vec3(x, y, z);
}

void Drawable::updateModelMatrix(){
    // Creates the model matrix from the Drawables position and rotation.
    glm::mat4 translation_matrix = glm::translate(glm::mat4(), position);

    model_matrix = rotation_matrix;
    model_matrix = translation_matrix * model_matrix;
}

glm::vec2 Drawable::getScreenPosition(Camera& camera) {
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = camera.getProjectionMatrix();

    return GLMHelpers::calculateScreenPosition(proj, view, position);

}
