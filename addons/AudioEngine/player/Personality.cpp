//
//  Personality.cpp
//  audioOutputExample
//
//  Created by Marek Bereza on 02/08/2011.
//

#include "Personality.h"
#include "OneShot.h"
#include "Looper.h"

/**
 * This loads in all the sounds to ram. Any 
 * duplicate files will only be loaded once
 * across the whole system.
 */
void Personality::loadFromXml(string xmlFile) {
    ofxXmlSettings xml;
    xml.loadFile(xmlFile);
    loadType("oneshot", xml);
    loadType("looper", xml);
}


void Personality::loadType(string type, ofxXmlSettings &xml) {
    int numOfThisType = xml.getNumTags(type);
    for(int i = 0; i < numOfThisType; i++) {
        Player *p = createType(type);
        p->loadFromXml(xml, type, i);
        players.push_back(p);
    }
}

Player *Personality::createType(string type) {
    if(type=="oneshot") {
        return new OneShot();
    } else if(type=="looper") {
        return new Looper();
    }
}
/**
 * This starts the personality playing.
 */
void Personality::fadeIn(float duration) {
    for(int i = 0; i < players.size(); i++) {
        players[i]->fadeIn(duration);
    }
}

/**
 * This fades out the personality.
 */
void Personality::fadeOut(float duration) {
    for(int i = 0; i < players.size(); i++) {
        players[i]->fadeOut(duration);
    }
}


/**
 * This ticks over the sequencing part
 */
void Personality::tick() {
    for(int i = 0; i < players.size(); i++) {
        players[i]->tick();
    }
}


Personality::~Personality() {
    for(int i = 0; i < players.size(); i++) {
        delete players[i];
    }
}