//
//  CallToAction.cpp
//  WailWall
//
//  Created by Joel Lewis on 20/02/2012.
//  Copyright (c) 2012 Hellicar & Lewis. All rights reserved.
//

#include "CallToAction.h"

#include "ofMain.h"

	
	// call this at the beginning
void CallToAction::setup(ofxWWTweetParticleManager *manager) {
	this->manager = manager;
	lastInteractionTime = ofGetElapsedTimef();
	oldLastInteractionTime = lastInteractionTime;
	image.loadImage("callToAction.png");
	image.setAnchorPercent(0.5, 0.5);
	fadeout = 1;
}
	
	// call this when someone interacts.
void CallToAction::justInteracted() {
	lastInteractionTime = ofGetElapsedTimef();
}

// call this every draw. The method
// will decide for itself when to draw.
void CallToAction::draw() {
	// Marek commented this out because we don't want a call to action anymore
/*
	float fadeInTime = 3;
	// this is the time since the last interaction
	float t = ofGetElapsedTimef() - lastInteractionTime - manager->callToActionTime;

	
	if(t<0) { // fading out
		fadeout-= 0.05;
		if(fadeout>0) {
			t = ofGetElapsedTimef() - oldLastInteractionTime - manager->callToActionTime;
		}
			// now adjust t to use the oldLastInteractionTime
	} else { // fading in
		fadeout = 1;
		oldLastInteractionTime = lastInteractionTime;
	}
	
		
	if(t>0) {

		float initialAlpha = ofMap(t, 0, fadeInTime, 0, 1, true);
		float alpha = ofMap(sin(t/5.f), -1, 1, 0.6, 9);
		
		
		
		glColor4f(1, 1, 1, alpha * initialAlpha * fadeout);
		
		float scale = ofMap(sin(t/4.f), -1, 1, 0.9, 1.1);
		float dy = sin(t/6.f) * 200;
		float dx = sin(t/7.f) * 300;
		image.draw(manager->simulationWidth/2 + dx , manager->simulationHeight/2 + dy, image.getWidth()*scale, image.getHeight()*scale);
	}
*/
}
