//
//  OneShot.cpp
//  audioOutputExample
//
//  Created by Marek Bereza on 02/08/2011.
//

#include "OneShot.h"


void OneShot::loadFromXml(ofxXmlSettings &xml, string tagName, int which) {
    
    // find out which outputs we want
    output1 = xml.getAttribute(tagName, "output1",  0, which);
    output2 = xml.getAttribute(tagName, "output2", -1, which);
    // default channels are 0 and -1 (i.e. just 0 - minus one is no speaker)
    
    minInterval = xml.getAttribute(tagName, "minInterval", 2.f, which);
    maxInterval = xml.getAttribute(tagName, "maxInterval", 3.f, which);
    
    minVolume = xml.getAttribute(tagName, "minVolume", 1.f, which);
    maxVolume = xml.getAttribute(tagName, "maxVolume", 1.f, which);
    
    
    minPan = xml.getAttribute(tagName, "minPan", 0.f, which);
    maxPan = xml.getAttribute(tagName, "maxPan", 1.f, which);
    
    // the pan should be 0 when mono
    if(output2==-1) minPan = maxPan = 0;        
    
    
    
    xml.pushTag(tagName, which);
    loadSounds(xml);
    
    // remember to restore the xml
    xml.popTag();
}



void OneShot::loadSounds(ofxXmlSettings &xml) {
    int numSounds = xml.getNumTags("sound");
    
    // load each sound in the xml
    for(int i = 0; i < numSounds; i++) {
        
        
        // test to see if we're loading files or folders
        if(xml.attributeExists("sound", "file", i)) {
            string filePath = ofToDataPath(xml.getAttribute("sound", "file", "", i));
            sounds.push_back(audio::loadSample(filePath));
        } else if(xml.attributeExists("sound", "folder", i)) {
            ofDirectory dir;
            dir.allowExt("wav");
            int numEntries = dir.listDir(xml.getAttribute("sound", "folder", "", i));
            for(int j = 0; j < numEntries; j++) {
                string filePath = ofToDataPath(dir.getPath(j));
                sounds.push_back(audio::loadSample(filePath));
            }
            
        }
    }
}


void OneShot::fadeIn(float duration) {
    fader.setValue(0);
    fader.fadeTo(1, duration);
    lastTimePlayedASound = ofGetElapsedTimef();
    interval = minInterval;
}


void OneShot::fadeOut(float duration) {
    fader.fadeTo(0, duration);
}

void OneShot::tick() {
    // if we're due to play a sound, create a new interval
    if(fader.getValue()>0 && ofGetElapsedTimef() - lastTimePlayedASound>interval) {
        interval = ofRandom(minInterval, maxInterval);
        lastTimePlayedASound = ofGetElapsedTimef();
        playRandomSound();
    }
}

void OneShot::playRandomSound() {
    // chose a random sample to play
    int whichSoundToPlay = rand()%sounds.size();
    
    
    // create a player for it (gets auto-destroyed when it finishes playing)
    audio::PlayerRef player = audio::createPlayer(sounds[whichSoundToPlay]);
    
    // assign its outputs
    if(output2==-1) {
        audio::setOutput(player, output1, -1);
    } else {
        audio::setOutput(player, output1, output2);
    }
    
    // set the volume and pan
    audio::setMix(player, ofRandom(minVolume, maxVolume)*fader.getValue(), ofRandom(minPan, maxPan));
    
    // jitter the delay in playing the sound file a little bit.
    audio::play(player, ofRandom(0, 0.05));
}
