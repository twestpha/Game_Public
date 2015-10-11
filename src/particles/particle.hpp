// particle.h
// Trevor Westphal

#ifndef Particle_h
#define Particle_h

#include <glm/glm.hpp>
#include <math.h>

#include "drawable.hpp"
#include "camera.hpp"
#include "mesh.hpp"

class Particle : public Drawable {
public:
    enum class ScalingOption { SCALE_UP_WITH_AGE, SCALE_SMOKE, SCALE_DOWN_WITH_AGE, SCALE_NONE};
    enum class FadingOption { FADE_OUT_WITH_AGE, FADE_IN_WITH_AGE, FADE_SMOKE, FADE_NONE};

    Particle(Mesh&, Shader& shader);
    Particle(Mesh&, Shader& shader, glm::vec3, GLfloat);
    Drawable* clone();

    // ~Particle();

    void setInitialValues(glm::vec3, glm::vec3, glm::vec3, float, int, ScalingOption, FadingOption);
    void update();
    void draw();
    bool isDead();
    void enablePhysics(float);

    string asJsonString();

private:
    void updateUniformData();

    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 dir;

    float rotationSpeed;
    float plane_rotation;
    float elasticity;
    float opacity;

    bool physicsEnabled;

    ScalingOption scaleWithAge;
    FadingOption alphaWithAge;

    int lifespan;
    int age;

};
#endif
