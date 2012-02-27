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
	void update();
	void draw();
	bool mustDoSomething();

	void setDoingSomethingForMillis(float millis);
	bool isDoingSomething();
	
	void reset();
	int getState();
	void setState(int state);
private:
	// 0 - doing nothing; handling interaction
	// 1 - doing something for x-millis
	// 2 - the x-millis are complete.. waiting for a reset.
	int state; 
	int do_something_until;
	bool is_doing_something;
	bool must_do_something; // used for call to action before, now to enable screenshots
	float lastInteractionTime;
	float oldLastInteractionTime;
	ofImage image;
	ofxWWTweetParticleManager *manager;
	float fadeout;
};

inline int CallToAction::getState() {
	return state;
}

inline void CallToAction::setState(int s) {
	state = s;
}