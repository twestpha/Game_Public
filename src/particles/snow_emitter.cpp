// snow_emitter.cpp
// Trevor Westphal

#include "snow_emitter.hpp"

SnowEmitter::SnowEmitter(const Json::Value& emitter_json) : Emitter(emitter_json) {

    initialize();
}

SnowEmitter::SnowEmitter(glm::vec3 position) : SnowEmitter(Shader("shaders/particle.vs",
    "shaders/particle.fs"), position) {;}


SnowEmitter::SnowEmitter(Shader shader, glm::vec3 position) : Emitter(shader, position){

    initialize();
}

void SnowEmitter::initialize(){
    // Hardcoded snow particle texture
    particle_texture = Texture("res/textures/snow_part.png");

    // Hardcoded density, maximum, and lifespan
    this->maxParticles = 1000;
    this->lifespan = 1000;
    this->density = (this->maxParticles)/(this->lifespan);
}

void SnowEmitter::prepareParticles(){
    for(int i(0); i < density; ++i){

        // range -10.0 < x < 10.0
        float random1 = ((rand()%100)/100.0f)*20.0f - 10.0f;

        // range -20.0 < x < 0.0
        float random2 = ((rand()%100)/100.0f)*-20.0f;

        float random3 = 0.0f;
        if(rand()%10 == 1){
            random3 = ((rand()%100)/100.0f)*0.01f - 0.005;
        }

        glm::vec3 newPosition(random1, 1.0f, random2+5.0f);
        // newPosition += camera->getPosition();

        glm::vec3 velocity(random3, -0.02, 0.0f);
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
            ptr->setInitialValues(newPosition, velocity, acceleration, 0.0f, lifespan, Particle::ScalingOption::SCALE_NONE, Particle::FadingOption::FADE_OUT_WITH_AGE);
            particles.push_back(ptr);
        }
    }
}
