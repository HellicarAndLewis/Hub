//
//  Looper.h
//  audioOutputExample
//
//  Created by Marek Bereza on 02/08/2011.
//

#pragma once

#include "Player.h"
#include "Fader.h"
#include "SineFader.h"
#include "AudioSystem.h"

class Looper: public Player {
public: 
    ~Looper();
    void loadFromXml(ofxXmlSettings &xml, string tagName, int which);    
    void fadeIn(float duration);    
    void fadeOut(float duration);    
    void tick();
    
    
    
private:
    void loadLoops(ofxXmlSettings &xml);
    Fader masterFader;
    vector<SineFader> faders;
	vector<audio::PlayerRef> channels;
    vector<audio::SampleRef> loops;
    vector<float> pans;
    
    // which speakers to play the sound out of
    int output1;
    int output2;

};