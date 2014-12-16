// smoke_emitter.cpp
// Trevor Westphal

#include <math.h>

#include "smoke_emitter.h"

SmokeEmitter::SmokeEmitter(GLuint shader_program, glm::vec3 position, float radius) : Emitter(shader_program, position){
    // Sets the smoke's radius
    this->radius = radius;

    // Hardcoded smoke particle texture
    GLuint emit = TextureLoader::loadTextureFromFile("res/textures/smoke_part.png", GL_LINEAR);
    texture_set = new TextureSet(0, 0, 0, emit);

    // Hardcoded density, maximum, and lifespan
    this->maxParticles = 100;
    this->lifespan = 2000;
    this->density = 1;
    // this->density = (this->maxParticles)/(this->lifespan);
}

void SmokeEmitter::prepareParticles(Camera* camera){
    
    if(count <  20){
        count++;
        return;
    }

    count = 0;

    for(int i(0); i < density; ++i){

        // range 0.0 < x < 6.28 (for theta)
        float randomTheta = ((rand()%100)/100.0f)*6.28;

        // range 0.0 < x < radius
        float randomRadius = ((rand()%100)/100.0f)*radius;

        // range 0.0 < x < 0.005
        float randomRotation = ((rand()%100)/10000.0f) - 0.005;

        // Generate particles in a circle, then transform the generated coordinates
        // to the actual emitter position
        glm::vec3 newPosition(randomRadius*cos(randomTheta), 0.0f, randomRadius*sin(randomTheta));
        newPosition += position;

        glm::vec3 velocity(0.0f, 0.01f, 0.0f);
        glm::vec3 acceleration(0.0f, 0.0f, 0.0f);
        
        // Particle recycling!
        // Weird that the pointer must be explicitly set to 0, but crashes without this
        Particle* ptr = 0; 
        if(particles.size() < maxParticles){
            ptr = new Particle(billboard, shader_program);
            ptr->attachTextureSet(*texture_set);
        } 
        if(particles.size() > 0 && particles[0]->isDead()){
            ptr = particles[0];
            particles.pop_front();
        }
        if(ptr){
            ptr->setInitialValues(newPosition, velocity, acceleration, randomRotation, lifespan, Particle::ScalingOption::SCALE_SMOKE, Particle::FadingOption::FADE_OUT_WITH_AGE);
            particles.push_back(ptr);
        }
    }
}