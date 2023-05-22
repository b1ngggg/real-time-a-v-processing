#pragma once

#include "ofMain.h"
#include "ofxMaxim.h"
#include "ofxStk.h"
#include "ofxGui.h"
#include "ParticleSystem.h"

class ofApp : public ofBaseApp{
public:
    ofApp();
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void mouseMoved(int x, int y);

    // These two functions deal with audio input and output
    void audioIn(ofSoundBuffer &input);
    void audioOut(ofSoundBuffer &output);

    // These two functions control the recording process
    void startRecording();
    void stopRecording();

    ParticleSystem ps;

    // SoundStream, Maximilian and STK objects
    ofSoundStream soundStream;
    maxiFilter filter;
    maxiDelayline delay;
    maxiOsc lfo;
    stk::PitShift pitchShifter;

    // Vector to hold recorded audio samples
    vector<double> recordedSamples;
    size_t playbackPos;

    // Vector to hold audio input
    vector<double> audioInput;

    // GUI panel and sliders
    ofxPanel gui;
    ofxFloatSlider filterFreqSlider;
    ofxFloatSlider filterResSlider;
    ofxFloatSlider delayTimeSlider;
    ofxFloatSlider feedbackSlider;
    ofxFloatSlider pitchShiftSlider;
    ofxFloatSlider reverbSlider;

    // Variables for graphics
    ofVec2f prevPos;
    float amplitude;
    ofVec2f newPos;
    ofColor color;
    float lineWidth;
    float rotation;
    ofVec2f scale;
    float x, y, radius;
    float maxTriangleSize;

    // Variables for audio processing
    double wave;
    double filterFreq, filterRes;
    double delayTime, feedback;
    double lfoSpeed, lfoDepth;
    double pitchShiftRatio;
    int sampleRate;
    bool bIsRecording;
};
