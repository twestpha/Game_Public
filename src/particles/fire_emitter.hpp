// fire_emitter.h
// Trevor Westphal
#ifndef FireEmitter_h
#define FireEmitter_h

#include <glm/glm.hpp>

#include "particles/emitter.hpp"
#include "camera.hpp"

class FireEmitter : public Emitter {
public:
    FireEmitter(const Json::Value& emitter_json);
    FireEmitter(glm::vec3, float);
    FireEmitter(Shader shader, glm::vec3, float);
private:
    float radius;
    void initialize(float);
    void prepareParticles();
};

#endif
