#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetWindowTitle("Minimal Synth Test");
    
	ofSetFrameRate(60);
	ofBackground(0);

	//  setup audio stream
	ofSoundStreamSettings settings;

	auto devices = soundStream.getDeviceList();
    
    // Print available devices
        for (auto & d : devices) {
            ofLogNotice() << d;
        }
    
    // Find "Built-in Output"
    for (auto & d : devices) {
        if (d.deviceID == 130) {
            settings.setOutDevice(d);
            ofLogNotice() << "Using device: " << d.name;
        }
    }

	settings.setOutListener(this);
	settings.numOutputChannels = 2; // stereo output
	settings.numInputChannels = 0;
	settings.sampleRate = sampleRate;
	settings.bufferSize = 1024;
	settings.numBuffers = 4;
	soundStream.setup(settings);

	// set initial increment
//	phaseInc = (TWO_PI * frequency) / sampleRate;
}

//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(255);
	ofDrawBitmapString("Audio Output Test - 440hz sine wave", 20, 20);
}

void ofApp::audioOut(ofSoundBuffer & outBuffer) {
    
    ofLogNotice() << "audioOut called, frames: " << outBuffer.getNumFrames();
    
    float phaseInc = (TWO_PI * frequency) / 48000.0f;
    
    for (size_t i = 0; i < outBuffer.getNumFrames(); i++) {
        float sample = sin(phase) * 0.2f;  // 0.2 to avoid clipping
        phase += phaseInc;
        if (phase > TWO_PI) phase -= TWO_PI;
        
        // stereo: same sample in L and R
        outBuffer[i * 2]  = sample; // left
        outBuffer[i * 2 + 1] = sample; // right
        
//        for (size_t channel = 0; channel < outBuffer.getNumChannels(); channel++) {
//            outBuffer[i * outBuffer.getNumChannels() + channel] = sample;
        }
    }
