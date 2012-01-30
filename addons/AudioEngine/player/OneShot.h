//
//  OneShot.h
//  audioOutputExample
//
//  Created by Marek Bereza on 02/08/2011.
//

#pragma once

#include "Player.h"
#include "AudioSystem.h"
#include "Fader.h"
class OneShot: public Player {
public:
    virtual void loadFromXml(ofxXmlSettings &xml, string tagName, int which);    
    
    
    
    
    void fadeIn(float duration);    
    
    void fadeOut(float duration);    
    void tick();    

private:
 
    void playRandomSound();
    void loadSounds(ofxXmlSettings &xml);    
    
    Fader fader;
    vector<audio::SampleRef> sounds;
    
    // which speakers to play the sound out of
    int output1;
    int output2;
    
    // how often to randomly play a sound
    float minInterval;
    float maxInterval;
    
    float minVolume;
    float maxVolume;
    
    float minPan;
    float maxPan;
    
    // currently, how long to wait till playing the next sound
    float interval;
    
    float lastTimePlayedASound;
};