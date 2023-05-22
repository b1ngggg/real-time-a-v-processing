#include "ParticleSystem.h"

ParticleSystem::ParticleSystem(ofVec2f location) : origin(location) {}

void ParticleSystem::addParticle() {
    Particle p;
    ofVec2f vel = ofVec2f(ofRandom(-1,1), ofRandom(-2,0));
    p.setup(origin, vel);
    particles.push_back(p);
}

void ParticleSystem::run() {
    for (int i = particles.size()-1; i >= 0; i--){
        particles[i].run();
        if (particles[i].isDead()) {
            particles.erase(particles.begin()+i);
        }
    }
}
