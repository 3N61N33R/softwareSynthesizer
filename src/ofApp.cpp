#include "ofApp.h"

// Helper function to map a key to a waveform type based on its row
int getWaveformForRow(int key)
{
    // Top row (QWERTY...) -> Sawtooth
    if (string("qwertyuiop").find(tolower(key)) != string::npos)
        return 2;
    // Middle row (ASDF...) -> Square
    if (string("asdfghjkl").find(tolower(key)) != string::npos)
        return 1;
    // Bottom row (ZXCVB...) -> Sine
    if (string("zxcvbnm").find(tolower(key)) != string::npos)
        return 0;
    return -1; // Not a valid key
}

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetWindowTitle("The Chromatic Typewriter");
    ofSetFrameRate(60);
    ofBackground(30, 30, 50); // A darker, more stylish background

    // --- Setup Key-to-Frequency Map ---
    // A simple chromatic scale starting from C3
    float baseFreq = 130.81f; // C3
    string allKeys = "zxcvbnmasdfghjklqwertyuiop";
    for (int i = 0; i < allKeys.length(); i++)
    {
        keyFrequencies[allKeys[i]] = baseFreq * pow(2.0, i / 12.0);
    }

    // --- Setup Audio Stream ---
    ofSoundStreamSettings settings;
    settings.setOutListener(this);
    settings.numOutputChannels = 2;
    settings.numInputChannels = 0;
    settings.sampleRate = sampleRate;
    settings.bufferSize = 512;
    settings.numBuffers = 4;
    soundStream.setup(settings);

    // Allocate space for waveform visualization
    waveformBuffer.resize(waveformBufferSize, 0.0f);
}

//--------------------------------------------------------------
void ofApp::update()
{
    // No logic needed here for this version
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetColor(255);
    ofDrawBitmapString("The Chromatic Typewriter", 20, 30);

    ofSetColor(200);
    ofDrawBitmapString("Top Row (QWERTY): Sawtooth Wave", 20, 60);
    ofDrawBitmapString("Mid Row (ASDF):  Square Wave", 20, 80);
    ofDrawBitmapString("Bot Row (ZXCV):  Sine Wave", 20, 100);
    ofDrawBitmapString("Play multiple keys at once!", 20, 120);

    // --- Draw Active Notes ---
    ofSetColor(255, 255, 0);
    string activeNotesStr = "Playing: ";
    audioMutex.lock();
    for (auto const &[key, val] : activeNotes)
    {
        activeNotesStr += (char)key;
        activeNotesStr += " ";
    }
    audioMutex.unlock();
    ofDrawBitmapString(activeNotesStr, 20, 160);

    // --- Draw Waveform Visualization ---
    ofNoFill();
    ofSetColor(100, 255, 150);
    ofSetLineWidth(2);
    ofBeginShape();
    audioMutex.lock();
    for (int i = 0; i < waveformBuffer.size(); i++)
    {
        float x = ofMap(i, 0, waveformBuffer.size() - 1, 0, ofGetWidth());
        float y = ofMap(waveformBuffer[i], -1.0, 1.0, ofGetHeight() * 0.75, ofGetHeight() * 0.25);
        ofVertex(x, y);
    }
    audioMutex.unlock();
    ofEndShape(false);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    int lowerKey = tolower(key);
    if (keyFrequencies.count(lowerKey))
    {
        int waveformType = getWaveformForRow(lowerKey);
        if (waveformType != -1)
        {
            std::lock_guard<std::mutex> lock(audioMutex);
            if (activeNotes.find(lowerKey) == activeNotes.end())
            {
                // Key is not already playing, so add it
                Note newNote;
                newNote.waveformType = waveformType;
                newNote.frequency = keyFrequencies[lowerKey];
                newNote.amplitude = 0.5f; // Fixed amplitude for now
                newNote.phase = 0.0f;
                activeNotes[lowerKey] = newNote;
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    int lowerKey = tolower(key);
    if (keyFrequencies.count(lowerKey))
    {
        std::lock_guard<std::mutex> lock(audioMutex);
        activeNotes.erase(lowerKey); // Remove the note for the released key
    }
}

//--------------------------------------------------------------
void ofApp::audioOut(ofSoundBuffer &outBuffer)
{
    float phaseInc;
    float sample;

    std::lock_guard<std::mutex> lock(audioMutex);

    // Zero out the buffer initially
    for (size_t i = 0; i < outBuffer.getNumFrames(); i++)
    {
        outBuffer[i * 2] = 0;
        outBuffer[i * 2 + 1] = 0;
    }

    // If there are no notes, we don't need to do anything else
    if (activeNotes.empty())
    {
        // Fill visualization buffer with silence
        for (size_t i = 0; i < waveformBuffer.size(); i++)
        {
            waveformBuffer[i] = 0.0f;
        }
        return;
    }

    // Iterate through all active notes and add their sound to the buffer
    for (auto it = activeNotes.begin(); it != activeNotes.end(); ++it)
    {
        Note &note = it->second;
        phaseInc = (TWO_PI * note.frequency) / sampleRate;

        for (size_t i = 0; i < outBuffer.getNumFrames(); i++)
        {
            // Generate sample based on waveform type
            switch (note.waveformType)
            {
            case 0: // Sine
                sample = sin(note.phase);
                break;
            case 1: // Square
                sample = (sin(note.phase) > 0) ? 1.0f : -1.0f;
                break;
            case 2: // Sawtooth
                sample = fmod(note.phase / TWO_PI, 1.0f) * 2.0f - 1.0f;
                break;
            default:
                sample = 0;
                break;
            }

            note.phase += phaseInc;
            if (note.phase > TWO_PI)
                note.phase -= TWO_PI;

            // Add the sample to the buffer (mix it in)
            // We divide by the number of notes to prevent clipping
            sample *= (note.amplitude / activeNotes.size());
            outBuffer[i * 2] += sample;     // Left channel
            outBuffer[i * 2 + 1] += sample; // Right channel
        }
    }

    // Copy the final mixed buffer to the visualization buffer
    for (size_t i = 0; i < outBuffer.getNumFrames() && i < waveformBuffer.size(); i++)
    {
        waveformBuffer[i] = outBuffer[i * 2]; // Just copy the left channel
    }
}
