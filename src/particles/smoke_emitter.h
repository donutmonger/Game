// smoke_emitter.h
// Trevor Westphal
#ifndef SmokeEmitter_h
#define SmokeEmitter_h

#include <glm/glm.hpp>

#include "particles/emitter.h"
#include "camera.h"

class SmokeEmitter : public Emitter {
public:
    SmokeEmitter(glm::vec3, float);
    SmokeEmitter(GLuint, glm::vec3, float);
private:
    float radius;
    int count;
    void prepareParticles(Camera*);
};

#endif
