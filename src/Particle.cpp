#include "Particle.h"

// Default constructor, sets the lifespan of the particle to 255.0
Particle::Particle() : lifespan(255.0) {}

// This function sets up the particle with a location (l) and velocity (v)
void Particle::setup(ofVec2f l, ofVec2f v) {
    pos = l;  // set the position to the provided location
    vel = v;  // set the velocity to the provided velocity
    size = ofRandom(1, 5);  // set the size to a random value between 1 and 5
    color = ofColor(ofRandom(255), ofRandom(255), ofRandom(255));  // set the color to a random RGB color
}

// This function updates the particle and then displays it
void Particle::run() {
    update();  // update the particle's state
    display();  // display the particle
}

// This function updates the position of the particle based on its velocity and decreases its lifespan
void Particle::update() {
    pos += vel;  // update the position based on the velocity
    lifespan -= 3.0;  // decrease the lifespan by 3.0
}

// This function draws the particle at its position with its color and size
void Particle::display() {
    ofSetColor(color, lifespan);  // set the color, the alpha component is set to the current lifespan
    ofDrawCircle(pos.x, pos.y, size);  // draw a circle at the particle's position with the particle's size
}

// This function checks if the particle is dead (if its lifespan is less than 0)
bool Particle::isDead() {
    return lifespan < 0.0;  // return true if the lifespan is less than 0, otherwise return false
}
