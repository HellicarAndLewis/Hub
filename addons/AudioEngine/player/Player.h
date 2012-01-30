//
//  Player.h
//  audioOutputExample
//
//  Created by Marek Bereza on 02/08/2011.
//

#pragma once
#include "ofxXmlSettings.h"

/**
 * This is the interface for a thing that triggers sound samples or controls loops
 */
class Player {
public:
    
    
    /** 
     * Loads content from xml - the xml object is pushed to the level below this tag
     * so you have to use the other parameters to actually get into it.
     */
    virtual void loadFromXml(ofxXmlSettings &xml, string tagName, int which) {}
    
    virtual void fadeIn(float duration) {}
    
    
    virtual void fadeOut(float duration) {}
    
    /** Ticks the internal sequencing. */
    virtual void tick() {}
    /**
     * You can dispatch events via personality to all players (such as particle collisions)
     * and deal with them as you wish.
     */
    virtual void eventReceived(string eventName, float *params, int numParams) {}
};