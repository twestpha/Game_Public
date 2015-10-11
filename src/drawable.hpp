#ifndef Drawable_h
#define Drawable_h

#include "includes/gl.hpp"
#include "includes/glm.hpp"

#include <cmath>

#include "mesh.hpp"
#include "camera.hpp"

#include "texture.hpp"


class Drawable {
public:
    Drawable () {;}
    Drawable(Mesh&, Shader& shader);
    Drawable(Mesh&, Shader& shader, glm::vec3, GLfloat);
    virtual Drawable* clone() = 0;

    virtual void draw();

    void setRotationEuler(GLfloat x, GLfloat y, GLfloat z);
    void setRotationEuler(glm::vec3 rotation);

    void rotateGlobalEuler(GLfloat x, GLfloat y, GLfloat z);
    void rotateGlobalEuler(glm::vec3 rotation);

    void rotateAxisAngle(glm::vec3 axis, GLfloat angle);

    void setPosition(glm::vec3 p);
    void setPosition(GLfloat x, GLfloat y, GLfloat z);

    void setScale(GLfloat s);
    void setShader(Shader& shader);

    void setDiffuse(Texture d);
    void setSpecular(Texture s);
    void setEmissive(Texture e);
    void setNormal(Texture n);

    virtual string asJsonString() = 0;

    glm::vec3 getPosition() {return position;}
    GLfloat getScale() {return scale;}
    Shader& getShader() {return *shader;}

    glm::vec2 getScreenPosition(Camera& camera);

protected:
    void load(Mesh&, Shader& shader, glm::vec3, GLfloat);
    void updateModelMatrix();

    virtual void bindTextures();
    virtual void setTextureLocations();
    virtual void updateUniformData() = 0;

    Mesh* mesh;

    Shader* shader;

    GLfloat scale;

    glm::vec3 position;
    glm::vec3 rotation;

    glm::mat4 model_matrix;
    glm::mat4 rotation_matrix;

    Texture diffuse;
    Texture specular;
    Texture emissive;
    Texture normal;
    Texture gloss;
};

#endif
