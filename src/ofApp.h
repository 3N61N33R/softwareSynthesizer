#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {

public:
	void setup() override;
	void update() override;
	void draw() override;

	void audioOut(ofSoundBuffer & outBuffer) override;

	ofSoundStream soundStream;

	// test tone
	float phase = 0.0f;
	float frequency = 440.0f;
	float sampleRate = 48000;

};
