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
	must_do_something = false;
	is_doing_something = false;
	do_something_until = 0;
	state = 0;
}


void CallToAction::setDoingSomethingForMillis(float millis) {
	do_something_until = ofGetElapsedTimeMillis() + millis;
}



bool CallToAction::isDoingSomething() {	
	return do_something_until > ofGetElapsedTimeMillis();
}

// Is called after we "did something" with the call to action.
void CallToAction::reset() {
	justInteracted();
	must_do_something = false;
	state = 0;
	do_something_until = 0;
	
}
	
// call this when someone interacts, only update when we're not already "doing something"
void CallToAction::justInteracted() {
	if(!is_doing_something) {
		lastInteractionTime = ofGetElapsedTimef();
	}
}

void CallToAction::update() {
	float t = ofGetElapsedTimef() - lastInteractionTime - manager->callToActionTime;
	must_do_something = t > 0;
}

// call this every draw. The method
// will decide for itself when to draw.
void CallToAction::draw() {
	if(!must_do_something) {
		return;
	}
	

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


bool CallToAction::mustDoSomething() {
	return must_do_something;
}
