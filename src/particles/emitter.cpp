#include "particles/emitter.hpp"

Emitter::Emitter(const Json::Value& emitter_json) {
    glm::vec3 pos;
    pos.x = emitter_json["position"]["x"].asFloat();
    pos.y = emitter_json["position"]["y"].asFloat();
    pos.z = emitter_json["position"]["z"].asFloat();

    initialize(Shader("shaders/particle.vs", "shaders/particle.fs"), pos);

}

Emitter::Emitter(glm::vec3 position) {
    initialize(Shader("shaders/particle.vs", "shaders/particle.fs"), position);
}

Emitter::Emitter(Shader shader, glm::vec3 position){
    initialize(shader, position);
}

Emitter::~Emitter(){
    // Deallocation is good!
    delete billboard;

    billboard = NULL;

    for(int i(0); i < particles.size(); ++i){
        delete particles[i];
        particles[i] = NULL;
    }
}

void Emitter::initialize(Shader shader, glm::vec3 pos) {
    this->position = pos;

    billboard = new PlaneMesh();

    particle_texture = Texture("res/textures/snow_part.png");

    this->maxParticles = 200;
    this->lifespan = 100;
    this->density = (this->maxParticles)/(this->lifespan);

    this->isShotgun = false;
    this->hasFired = false;

    this->shader = shader;
}

void Emitter::setParticleDensity(int density){
    // If the user does not explicitly set the density
    // using this function (thus creating different,
    // custom particle behavior), the emitter will
    // generate the density that will look best as per
    // the lifespan and the total number of particles
    // This will prevent live particles from being
    // recycled before it is their time
    this->density = density;
}

void Emitter::draw(){
    // Disable depth sorting when rendering
    // particles.
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    float new_time = GameClock::getInstance()->getDeltaTime();
    int frames = int((new_time*60.0f)) + 1;
    frames = std::max(frames, 0);

    for(int j = 0; j < frames; ++j){
        for(int i = 0; i < particles.size(); ++i){
            particles[i]->update();
        }
    }

    prepareParticles();
    for(int i = 0; i < particles.size(); ++i){
        particles[i]->draw();
    }

    // Re enable depth sorting for everything else
    // (really should not be here)
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
}

void Emitter::makeShotgun(){
    density = maxParticles;
    isShotgun = true;
    hasFired = false;
}

void Emitter::prepareParticles(){

    // Shotgun stuff
    if(hasFired && isShotgun){
        return;
    }
    hasFired = true;

    for(int i(0); i < density; ++i){
        // Shitty random generation for now
        float rand1 = ((rand()%100)/100.0f)-0.5f;
        float rand2 = ((rand()%100)/100.0f)-0.5f;
        float rand3 = ((rand()%100)/100.0f)-0.5f;

        glm::vec3 position(-1.3f+rand1, 0.0f, rand2);
        // Emitter parented to camera
        // position+=camera->getPosition();
        glm::vec3 velocity(0.0f, 0.01f+(0.01f*rand3), 0.0f);
        glm::vec3 acceleration(0.0f, 0.0f, 0.0f);

        // Random wind interaction for snow particles.
        // if (rand() % 1000){
        //     velocity += glm::vec3(0.001 * (rand() % 5), - abs(0.0005 * (rand() % 5)), 0.001 * (rand() % 5));
        // }

        float rotation = 0.0f;

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
            ptr->setInitialValues(position, velocity, acceleration, rotation, lifespan, Particle::ScalingOption::SCALE_DOWN_WITH_AGE, Particle::FadingOption::FADE_NONE);
            particles.push_back(ptr);
        }
    }
}
