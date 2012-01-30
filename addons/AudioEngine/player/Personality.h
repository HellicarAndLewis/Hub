//
//  Personality.h
//  audioOutputExample
//
//  Created by Marek Bereza on 02/08/2011.
//

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "Player.h"

class Personality {

public:
    
    ~Personality();
    
    /**
     * This loads in all the sounds to ram. Any 
     * duplicate files will only be loaded once
     * across the whole system.
     */
    void loadFromXml(string xmlFile);
    
    
    /**
     * This starts the personality playing.
     */
    void fadeIn(float duration = 1);
    
    /**
     * This fades out the personality.
     */
    void fadeOut(float duration = 1);
    
    /**
     * This ticks over the sequencing part
     */
    void tick();
    
protected:
    vector<Player*> players;

private:
    // creates a type of player based on a string
    Player *createType(string type);
    
    // finds all the tags called type in the xml
    // and loads them into the corresponding player
    // objects, as instantiated by createType(type)
    void loadType(string type, ofxXmlSettings &xml);
};


