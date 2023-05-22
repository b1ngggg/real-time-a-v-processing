#include "ofApp.h"
#define TWO_PI 6.283185307179586476925286766559

void ofApp::setup(){    // Setting up the initial state of the application

    ofSetFrameRate(60);
    ofBackground(0);
    
    sampleRate = 44100;
    int bufferSize = 512;
//    reverb = new Reverb();

    audioInput.assign(bufferSize, 0.0);
    
    soundStream.setup(2, 1, sampleRate, bufferSize, 4);

    pitchShifter.setShift(1.0);
    bIsRecording = false;
    

    
    playbackPos = 0;

    gui.setup();
    gui.add(filterFreqSlider.setup("Filter Freq", 20, 20, 20000));
    gui.add(filterResSlider.setup("Filter Res", 1, 1, 10));
    gui.add(delayTimeSlider.setup("Delay Time", 0.1, 0.1, 0.9));
    gui.add(feedbackSlider.setup("Feedback", 0.0, 0.0, 0.9));
    gui.add(pitchShiftSlider.setup("Pitch Shift", 1.0, 0.5, 2.0));
    
    ps = ParticleSystem(ofVec2f(ofGetWidth()/2, ofGetHeight()/2));
    prevPos.set(ofGetWidth()/2, ofGetHeight()/2);
    maxTriangleSize = 0.0;


}
ofApp::ofApp() : ps(ofVec2f(ofGetWidth()/2, ofGetHeight()/2)) {    // Initialize the particle system at the center of the window

    
}

void ofApp::update(){    // Update the application state on each frame

    pitchShiftRatio = ofMap(ofGetMouseY(), 0, ofGetHeight(), 0.5, 2.0, true);
    pitchShifter.setShift(pitchShiftRatio);
    
    filterFreqSlider = filterFreq;
    filterResSlider = filterRes;
    delayTimeSlider = delayTime;
    feedbackSlider = feedback;
    pitchShiftSlider = pitchShiftRatio;
}

void ofApp::audioIn(ofSoundBuffer &input){    // Handle incoming audio

    
    amplitude = 0;

    for (size_t i = 0; i < input.getNumFrames(); i++){
        audioInput[i] = input[i*input.getNumChannels()];
        amplitude += abs(audioInput[i]);


        if (bIsRecording) {
            recordedSamples.push_back(input[i*input.getNumChannels()]);
        }
    }
    amplitude /= input.getNumFrames();
    int numParticles = ofMap(amplitude, 0, 1, 0, 100);  // increase the max number of particles
     for (int i = 0; i < numParticles; i++) {
         ps.addParticle();
     }
    x = ofMap(amplitude, 0, 1, 0, ofGetWidth(), true);
    y = ofMap(filterRes, 1, 10, 0, ofGetHeight(), true);
    radius = ofMap(delayTime, 0.1, 0.9, 10, 100, true);
    
    color = ofColor(
        ofMap(filterFreq, 20, 20000, 0, 255, true),
        ofRandom(255),
        ofRandom(255),
        ofMap(pitchShiftRatio, 0.5, 2.0, 0, 255, true)
    );
}



void ofApp::audioOut(ofSoundBuffer &output){    // Handle outgoing audio

        filterFreq = ofMap(ofGetMouseX(), 0, ofGetWidth(), 20, 20000, true);
        filterRes = ofMap(ofGetMouseY(), 0, ofGetHeight(), 1, 10, true);
        delayTime = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0.1, 0.9, true);
        feedback = ofMap(ofGetMouseY(), 0, ofGetHeight(), 0.0, 0.9, true);
        pitchShiftRatio = ofMap(ofGetMouseX(), 0, ofGetWidth(), 0.5, 2.0, true);
    
    for (size_t i = 0; i < output.getNumFrames(); i++){
        double sample;

        if (!bIsRecording && playbackPos < recordedSamples.size()) {
            sample = recordedSamples[playbackPos++];
        } else if (!bIsRecording) {
            sample = audioInput[i];
        } else {
            sample = 0.0;
        }

        
        double filtered = filter.lores(sample, filterFreq, filterRes);
        double delayed = delay.dl(filtered, delayTime * sampleRate, feedback);
        double shifted = pitchShifter.tick(delayed);
        
        double pan = (lfo.sinewave(0.5) + 1) / 2;
        double left = shifted * (1 - pan);
        double right = shifted * pan;
        
        output[i*output.getNumChannels()] = left;
        output[i*output.getNumChannels() + 1] = right;
        
        amplitude = 0;
        for (size_t i = 0; i < output.getNumFrames(); i++){
            amplitude += abs(output[i*output.getNumChannels()]);
        }
        amplitude /= output.getNumFrames();

        float angle = ofRandom(0, TWO_PI);
        float length = ofMap(delayTime, 0.1, 0.9, 0, 100);  // use delayTime to adjust length
        newPos = prevPos + ofVec2f(cos(angle + feedback * TWO_PI), sin(angle + feedback * TWO_PI)) * length; // use feedback to influence direction
        // Ensure newPos is within screen bounds
         newPos.x = ofClamp(newPos.x, 0, ofGetWidth());
         newPos.y = ofClamp(newPos.y, 0, ofGetHeight());
        color = ofColor(ofMap(filterFreq, 20, 20000, 0, 255),  // use filterFreq to adjust color
                   ofRandom(255),
                   ofRandom(255),
                   ofMap(pitchShiftRatio, 0.5, 2.0, 0, 255)); // use pitch to control opacity
        lineWidth = ofMap(filterRes, 1, 10, 0, 10);  // use filterRes to control line thickness
        prevPos = newPos;
        rotation = ofMap(delayTime, 0.1, 0.9, 0, 360);  // use delayTime to adjust rotation
          scale.x = ofMap(filterRes, 1, 10, 0.5, 2);  // use filterRes to control x scaling
          scale.y = ofMap(pitchShiftRatio, 0.5, 2.0, 0.5, 2); // use pitch to control y scaling
        maxTriangleSize = ofMap(delayTime, 0.1, 0.9, 10, 50, true);

        
    }
}
void ofApp::draw(){
    gui.draw();
    ofSetColor(255); // set color to white
        string msg = "Press 'r' to start recording, 's' to stop recording, 'p' to playback recorded audio.";
        ofDrawBitmapString(msg, 15, ofGetHeight() - 30); // print the instructions at the bottom of the screen
    ps.run();
    ofSetColor(color);
    ofSetLineWidth(lineWidth);
    ofDrawLine(prevPos.x, prevPos.y, newPos.x, newPos.y);
    ofSetColor(color);
    ofSetLineWidth(lineWidth);
//    // Draw a line from the center of the screen to the new position
//    ofDrawLine(ofGetWidth()/2, ofGetHeight()/2, newPos.x, newPos.y);
    // Draw lines
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        ofRotateZDeg(rotation);
        ofScale(scale.x, scale.y);

        ofSetColor(color);
        ofSetLineWidth(lineWidth);
        int numLines = 10;
        float angleStep = 360.0 / numLines;
        for(int i = 0; i < numLines; i++) {
            ofPushMatrix();
            ofRotateZDeg(angleStep * i);
            ofDrawLine(0, 0, newPos.x, newPos.y);
            ofPopMatrix();
        }
        ofPopMatrix();
    
    
    //draw circle
    ofPushMatrix();
        ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

        int numCircles = 50;
        float maxRadius = ofMap(amplitude, 0, 1, 50, 200, true);
        float dr = maxRadius / numCircles;

        for (int i = 0; i < numCircles; i++) {
            float radius = i * dr;
            ofSetColor(
                ofMap(radius, 0, maxRadius, 0, 255, true),
                ofMap(amplitude, 0, 1, 0, 255, true),
                ofRandom(255),
                ofMap(amplitude, 0, 1, 0, 255, true)
            );
            ofNoFill();
            ofDrawCircle(0, 0, radius);
        }

        ofPopMatrix();
    
    
    // Add a grid of squares
        ofPushMatrix();
        ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);

        int gridSize = 10; // Change this value to alter the grid density
        float maxSquareSize = ofMap(amplitude, 0, 1, 10, 50, true);

        for (int i = -gridSize; i <= gridSize; i++) {
            for (int j = -gridSize; j <= gridSize; j++) {
                float squareSize = maxSquareSize * (1 - ofDist(0, 0, i, j) / (2 * gridSize));
                ofSetColor(
                    ofMap(i, -gridSize, gridSize, 0, 255, true),
                    ofMap(j, -gridSize, gridSize, 0, 255, true),
                    ofMap(squareSize, 0, maxSquareSize, 0, 255, true),
                    ofMap(amplitude, 0, 1, 0, 255, true)
                );
                ofFill();
                ofDrawRectangle(i * maxSquareSize, j * maxSquareSize, squareSize, squareSize);
            }
        }

        ofPopMatrix();
//

}


void ofApp::startRecording() {    // Set the recording state to true and clear the recorded samples vector

    bIsRecording = true;
    recordedSamples.clear();
    playbackPos = 0;
}

void ofApp::stopRecording() {    // Set the recording state to false

    bIsRecording = false;
}

void ofApp::keyPressed(int key){    // Handle key press events

    if(key == 'r'){
        startRecording();
    }
    if(key == 's'){
        stopRecording();
    }
    if(key == 'p'){
        playbackPos = 0;
    }
}

void ofApp::mouseMoved(int x, int y){    // Update the origin of the particle system when the mouse is moved

    ps.origin.set(x, y);
}

