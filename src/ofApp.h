#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {

public:
	void setup() override;
	void update() override;
	void draw() override;

    void keyPressed(int key) override; // <-- handle keyboard
	void audioOut(ofSoundBuffer & outBuffer) override;

	ofSoundStream soundStream;

	// Synth statetest tone
	float phase = 0.0f;
	float frequency = 440.0f;
	float sampleRate = 48000;
    
    int waveformType = 0; // 0 = sine, 1 = square, 2 = sawtooth

};
