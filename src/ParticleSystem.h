#pragma once

#include "ofMain.h"
#include "Particle.h"

class ParticleSystem {
public:
    vector<Particle> particles;
    ofVec2f origin;

    ParticleSystem(ofVec2f location);

    void addParticle();
    void run();
};
