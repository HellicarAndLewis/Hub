//
//  Looper.cpp
//  audioOutputExample
//
//  Created by Marek Bereza on 02/08/2011.
//  Copyright 2011 MAZBOX LTD. All rights reserved.
//

#include "Looper.h"



void Looper::loadFromXml(ofxXmlSettings &xml, string tagName, int which) {
    
    
    // find out which outputs we want
    output1 = xml.getAttribute(tagName, "output1",  0, which);
    output2 = xml.getAttribute(tagName, "output2", -1, which);

    xml.pushTag(tagName, which);
    loadLoops(xml);
    xml.popTag();
    
    
}

void Looper::fadeIn(float duration) {
    
    // fade out any old samples
    fadeOut(duration);
    
    // create new players and faders
    for(int i = 0; i < loops.size(); i++) {
        channels.push_back(audio::createPlayer(loops[i]));
        
        
        // start playing the sound
        audio::play(channels.back(), 0.01); // need the 0.01 to stop a weird clicking noise. Bad hack, but I'm in a rush!
        
        audio::setLooping(channels.back(), true);
        
        // set the volume to zero
        audio::setMix(channels.back(), 0, pans[i]);

        audio::setOutput(channels.back(), output1, output2); // output2 defaults to -1 if not set, which is what audio takes to mean not set too.
        
        
        faders[i].reset();
        
    }
    masterFader.setValue(0);
    masterFader.fadeTo(1, duration);
}

void Looper::fadeOut(float duration) {
    for(int i = 0; i < channels.size(); i++) {
        audio::fadeOut(channels[i], duration);
    }
    channels.clear();
    masterFader.fadeTo(0,duration);
}

Looper::~Looper() {
    fadeOut(0);
}

void Looper::tick() {
    float masterVolume = masterFader.getValue();
    if(masterVolume>0) {
        for(int i = 0; i < channels.size(); i++) {
            float vol = masterVolume*faders[i].getValue();

            audio::setVolume(channels[i], vol );
        }
    }
}

void Looper::loadLoops(ofxXmlSettings &xml) {
    int numLoops = xml.getNumTags("loop");
    
    // load each sound in the xml
    for(int i = 0; i < numLoops; i++) {
        
        // test to see if we're loading files or folders
        string filePath = ofToDataPath(xml.getAttribute("loop", "file", "", i));
        loops.push_back(audio::loadSample(filePath));
        faders.push_back(SineFader());
        faders.back().modulation = xml.getAttribute("loop", "modulation", 0.f, i);
        faders.back().volume     = xml.getAttribute("loop", "volume", 1.f, i);
        faders.back().frequency  = xml.getAttribute("loop", "frequency", ofRandom(0.001, 0.01), i); // nice random (low) frequency
        faders.back().phase      = xml.getAttribute("loop", "phase", 1.f, i);
        float pan = xml.getAttribute("loop", "pan", 0.5, i);
        if(pan==0.5 && output2==-1) pan = 0;
        pans.push_back(pan);
    }
}


