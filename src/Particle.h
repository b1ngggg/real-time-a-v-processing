//
#pragma once

#include "ofMain.h"

class Particle {
public:
    ofVec2f pos;
    ofVec2f vel;
    float lifespan;
    float size;
    ofColor color;


    Particle();

    void setup(ofVec2f l, ofVec2f v);
    void run();
    void update();
    void display();
    bool isDead();
};
