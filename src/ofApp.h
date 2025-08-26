#pragma once

#include "ofMain.h"
#include <map>

// A structure to hold the properties of a single active note
struct Note
{
	int waveformType; // 0: Sine, 1: Square, 2: Sawtooth
	float frequency;
	float amplitude;
	float phase;
};

class ofApp : public ofBaseApp
{

public:
	void setup() override;
	void update() override;
	void draw() override;

	void keyPressed(int key) override;
	void keyReleased(int key) override;
	void audioOut(ofSoundBuffer &outBuffer) override;

	ofSoundStream soundStream;

	float sampleRate = 48000;

	// Use a map to store currently active notes (keyed by the keyboard key)
	// This allows for polyphony
	std::map<int, Note> activeNotes;
	std::mutex audioMutex; // Protects activeNotes

	// Key-to-frequency mapping
	std::map<int, float> keyFrequencies;

	// Visualization
	std::vector<float> waveformBuffer; // stores recent samples for drawing
	int waveformBufferSize = 512;
};
