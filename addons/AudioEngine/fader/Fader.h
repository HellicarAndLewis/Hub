//
//  Fader.h
//  audioOutputExample
//
//  Created by Marek Bereza on 02/08/2011.
//

#pragma once
#include "ofMain.h"
#include "AbstractFader.h"
/**
 * Simple class that manages the timing of a fade between values.
 * A little bit like a simple tweener.
 */
class Fader: public AbstractFader {
public:
    
    void setValue(float value) {
        fadeFrom = value;
        fadeTarget = value;
        fadeDuration = 5;
        fadeStart = -5;
    }
    
    void fadeTo(float value, float duration) {
        fadeFrom = getValue();
        fadeTarget = value;
        fadeStart = ofGetElapsedTimef();
        fadeDuration = duration;
        
    }
    
    float getValue() {
        float t = ofGetElapsedTimef();
        if(t>fadeStart+fadeDuration) return fadeTarget; // a little bit faster than just calling ofMap all the time.
        return ofMap(t - fadeStart, 0, fadeDuration, fadeFrom, fadeTarget, true);
    }
    
private:
    float fadeFrom;
    float fadeTarget;
    float fadeStart;
    float fadeDuration;
};