#ifndef FlatDrawable_h
#define FlatDrawable_h

#include "includes/gl.hpp"
#include "includes/glm.hpp"

#include "mesh.hpp"
#include "flat_mesh.hpp"
#include "texture.hpp"

class FlatDrawable {
public:
    FlatDrawable() : FlatDrawable(1.0, 1.0, glm::vec2()){;}
    FlatDrawable(Shader shader) : FlatDrawable(shader, 1.0, 1.0, glm::vec2()){;}
    FlatDrawable(GLfloat, GLfloat, glm::vec2);
    FlatDrawable(Shader shader, GLfloat, GLfloat, glm::vec2);

    virtual void draw();
    virtual void attachTexture(Texture);

    virtual void setPosition(glm::vec2 p) {position = p;}
    virtual void setGLCoordinates(glm::vec2, glm::vec2){;}

    void setOpacity(float);
    void setShader(Shader shader);

protected:
    void load(Shader shader, GLfloat, GLfloat, glm::vec2);

    virtual void updateUniformData();

    FlatMesh* mesh;
    Shader shader;

    GLfloat width;
    GLfloat height;
    glm::vec2 position;

    Texture texture;

    float opacity;

    glm::mat3 transformation;
};

#endif
