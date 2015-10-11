// smoke_emitter.h
// Trevor Westphal
#ifndef SmokeEmitter_h
#define SmokeEmitter_h

#include <glm/glm.hpp>

#include "particles/emitter.hpp"
#include "camera.hpp"

class SmokeEmitter : public Emitter {
public:
    SmokeEmitter(const Json::Value& emitter_json);
    SmokeEmitter(glm::vec3, float);
    SmokeEmitter(Shader, glm::vec3, float);
private:
    float radius;
    int count;
    void initialize(float r);
    void prepareParticles();
};

#endif
