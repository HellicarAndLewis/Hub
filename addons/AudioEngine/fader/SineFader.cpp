//
//  SineFader.cpp
//  audioOutputExample
//
//  Created by Marek Bereza on 02/08/2011.
//  Copyright 2011 MAZBOX LTD. All rights reserved.
//

#include "SineFader.h"

#ifndef TWO_PI 
#define TWO_PI 6.2831853071f
#endif

SineFader::SineFader() {
    // give it a random phase offset
    frequency = 0.1;
    modulation = 1;
    volume = 1;
    phase = 0;
    reset();
}


float SineFader::getValue() {
    
    float t = ofGetElapsedTimef();
    float delta = t - lastTime;
    lastTime = t;
    
    float phaseIncrement = delta*frequency; // this is the phaseIncrement going from 0 to 1
    phaseIncrement *= TWO_PI; // make it into radians
    phase += phaseIncrement;
    while(phase>TWO_PI) phase -= TWO_PI;

    // sine that goes from between "volume*(1.f-modulation)" and "volume"
    
    return ofMap(sin(phase), -1, 1, volume*(1.f-modulation), volume);
}

void SineFader::reset() {
    phase = 0;
    lastTime = ofGetElapsedTimef();
}

