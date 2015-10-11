// fire_emitter.cpp
// Trevor Westphal

#include <math.h>

#include "fire_emitter.hpp"

FireEmitter::FireEmitter(const Json::Value& emitter_json) : Emitter(emitter_json) {

    initialize(emitter_json["radius"].asFloat());

}

FireEmitter::FireEmitter(glm::vec3 position, float radius) : FireEmitter(Shader("shaders/particle.vs",
    "shaders/particle.fs"), position, radius) {;}


FireEmitter::FireEmitter(Shader shader, glm::vec3 position, float radius) : Emitter(shader, position){
    initialize(radius);
}

void FireEmitter::initialize(float r) {
    // Sets the fire's radius
    this->radius = r;

    // Hardcoded fire particle texture
    particle_texture = Texture("res/textures/fire_part.png");

    // Hardcoded density, maximum, and lifespan
    this->maxParticles = 200;
    this->lifespan = 80;
    this->density = (this->maxParticles)/(this->lifespan);
}

void FireEmitter::prepareParticles(){
    for(int i(0); i < density; ++i){

        // range 0.0 < x < 6.28 (for theta)
        float randomTheta = ((rand()%100)/100.0f)*6.28;

        // range 0.0 < x < radius
        float randomRadius = ((rand()%100)/100.0f)*radius;

        // range 0.01 < x < 0.02
        float randomVelocity = 0.01f + ((rand()%100)/10000.0f);

        // Generate particles in a circle, then transform the generated coordinates
        // to the actual emitter position
        glm::vec3 newPosition(randomRadius*cos(randomTheta), 0.0f, randomRadius*sin(randomTheta));
        newPosition += position;

        glm::vec3 velocity(0.0f, randomVelocity, 0.0f);
        glm::vec3 acceleration(0.0f, 0.0f, 0.0f);

        // Particle recycling!
        // Weird that the pointer must be explicitly set to 0, but crashes without this
        Particle* ptr = 0;
        if(particles.size() < maxParticles){
            ptr = new Particle(*billboard, shader);
            ptr->setEmissive(particle_texture);
        }
        if(particles.size() > 0 && particles[0]->isDead()){
            ptr = particles[0];
            particles.pop_front();
        }
        if(ptr){
            ptr->setInitialValues(newPosition, velocity, acceleration, 0.0f, lifespan, Particle::ScalingOption::SCALE_DOWN_WITH_AGE, Particle::FadingOption::FADE_NONE);
            particles.push_back(ptr);
        }
    }
}
