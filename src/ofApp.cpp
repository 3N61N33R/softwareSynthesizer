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
    
    ofDrawBitmapString("Move mouse horizontally to change pitch (freq)", 20, 80);
    ofDrawBitmapString("Move mouse vertically to change volume", 20, 100);

    ofDrawBitmapString("Frequency: " + ofToString(frequency, 2) + " Hz", 20, 140);
    ofDrawBitmapString("Amplitude: " + ofToString(amplitude, 2), 20, 160);
    
    // Draw waveform
    waveformMutex.lock();
    ofNoFill();
    ofSetColor(0, 255, 100);
    ofBeginShape();
    
    for (int i = 0; i < waveform.size(); i++) {
        float x = ofMap(i, 0, waveform.size(), 0, ofGetWidth());
        float y = ofMap(waveform[i], -1.0, 1.0, ofGetHeight()/2 + 100, ofGetHeight()/2 - 100);
        ofVertex(x, y);
    }
    ofEndShape(false);
    waveformMutex.unlock();
    
}

void ofApp::keyPressed(int key) {
    if (key == '1') waveformType = 0;
    else if (key == '2') waveformType = 1;
    else if (key == '3') waveformType = 2;
}

void ofApp::mouseMoved(int x, int y) {
    // Map X position to frequency range (C2 ~ 65 Hz up to C7 ~ 2000 Hz)
    frequency = ofMap(x, 0, ofGetWidth(), 65.0f, 2000.0f, true);

    // Map Y position to amplitude (top loud = 1.0, bottom quiet = 0.0)
    amplitude = ofMap(y, 0, ofGetHeight(), 1.0f, 0.0f, true);
}


void ofApp::audioOut(ofSoundBuffer & outBuffer) {
    
    // uncomment this line to test if audioOut is being called
//    ofLogNotice() << "audioOut called, frames: " << outBuffer.getNumFrames();
    
    float phaseInc = (TWO_PI * frequency) / sampleRate;
    
    std::vector<float> localBuffer(outBuffer.getNumFrames());
    
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
        sample *= amplitude;
        
        // stereo: same sample in L and R
        outBuffer[i * 2]  = sample; // left
        outBuffer[i * 2 + 1] = sample; // right
        
        localBuffer[i] = sample; // copy to local buffer

        }
    
    // Copy recent samples into waveform buffer (thread-safe)
    waveformMutex.lock();
    for (size_t i = 0; i < localBuffer.size(); i++) {
        waveform[i % waveform.size()] = localBuffer[i];
    }
    waveformMutex.unlock();
    
}
