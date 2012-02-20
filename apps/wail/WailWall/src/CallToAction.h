//
//  CallToAction.h
//  WailWall
//
//  Created by Joel Lewis on 20/02/2012.
//  Copyright (c) 2012 Hellicar & Lewis. All rights reserved.
//
#pragma once
#include "ofxWWTweetParticleManager.h"
class CallToAction {
public:
	
	// call this at the beginning
	void setup(ofxWWTweetParticleManager *manager);
	
	// call this when someone interacts.
	void justInteracted();
	
	// call this every draw. The method
	// will decide for itself when to draw.
	void draw();
	
private:
	float lastInteractionTime;
	float oldLastInteractionTime;
	ofImage image;
	ofxWWTweetParticleManager *manager;
	float fadeout;
};