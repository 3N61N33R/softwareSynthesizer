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

}

//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofSetColor(255);
    ofDrawBitmapString("Press 1 = sine, 2 = square, 3 = sawtooth", 20, 20);

    string waveName = (waveformType == 0 ? "SINE" : (waveformType == 1 ? "SQUARE" : "SAWTOOTH"));
    ofDrawBitmapString("Current waveform: " + waveName, 20, 50);
    
}

void ofApp::keyPressed(int key) {
    if (key == '1') waveformType = 0;
    else if (key == '2') waveformType = 1;
    else if (key == '3') waveformType = 2;
}


void ofApp::audioOut(ofSoundBuffer & outBuffer) {
    
    // uncomment this line to test if audioOut is being called
//    ofLogNotice() << "audioOut called, frames: " << outBuffer.getNumFrames();
    
    float phaseInc = (TWO_PI * frequency) / 48000.0f;
    
    for (size_t i = 0; i < outBuffer.getNumFrames(); i++) {
        float sample = sin(phase);
        
        // Generate waveform depending on type
        if (waveformType == 0) {
            // Sine
            sample = sin(phase);
                }
        else if (waveformType == 1) {
            // Square
            sample = (sin(phase) > 0 ? 1.0f : -1.0f);
                }
        else if (waveformType == 2) {
            // Sawtooth
            sample = fmod(phase / TWO_PI, 1.0f) * 2.0f - 1.0f;
                }
        // advance phase
        phase += phaseInc;
        if (phase > TWO_PI) phase -= TWO_PI;
        
        // reduce gain to avoid clipping
        sample *= 0.2f;
        
        // stereo: same sample in L and R
        outBuffer[i * 2]  = sample; // left
        outBuffer[i * 2 + 1] = sample; // right

        }
    }
