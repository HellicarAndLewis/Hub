/**
 *  KinectTouchSimulator.cpp
 *
 *  Created by Marek Bereza on 01/02/2012.
 */

#include "KinectTouchSimulator.h"



KinectTouchSimulator::KinectTouchSimulator() {
	listener = NULL;
	mouseIsDown = false;
	enabled = false;
	idCounter = 0;
	touchDownTime = 0;
}
void KinectTouchSimulator::setEnabled(bool enabled) {
	this->enabled = enabled;
}
bool KinectTouchSimulator::getEnabled() {
	return enabled;
}
void KinectTouchSimulator::setListener(KinectTouchListener *listener) {
	this->listener = listener;
}

float KinectTouchSimulator::getTouchZ(float time) {
	float initialDescent = ofClamp(time*0.25, 0, 0.5);
	
	return initialDescent + (float)sin(time)*0.2f;
}

void KinectTouchSimulator::mousePressed(int x, int y) {
	if(!enabled || listener==NULL) return;
	idCounter++;
	
	mouseIsDown = true;
	touchDownTime = ofGetElapsedTimef();
	
	touch.id = idCounter;
	touch.x = ofMap(x, 0, ofGetWidth(), 0, 1);
	touch.y = ofMap(y, 0, ofGetHeight(), 0, 1);
	touch.z = 0.75;
	touch.size = 0.15;
	listener->touchDown(touch);
	
}

void KinectTouchSimulator::mouseDragged(int x, int y) {

	if(!enabled || listener==NULL) return;
	
	
	ofVec3f oldTouch = ofVec3f(touch.x, touch.y, touch.z);
	touch.x = ofMap(x, 0, ofGetWidth(), 0, 1);
	touch.y = ofMap(y, 0, ofGetHeight(), 0, 1);
	touch.z = 0.75;
	//touch.z = getTouchZ(ofGetElapsedTimef() - touchDownTime);
	//touch.size = 0.01 + touch.z*0.3;
	touch.vel.x = touch.x - oldTouch.x;
	touch.vel.y = touch.y - oldTouch.y;
	touch.vel.z = touch.z - oldTouch.z;
	touch.size = 0.1;
	listener->touchMoved(touch);	

}

void KinectTouchSimulator::mouseReleased(int x, int y) {
	if(listener==NULL) return;
	if(!enabled && !mouseIsDown) return;
	mouseIsDown = false;
	
	touch.x = ofMap(x, 0, ofGetWidth(), 0, 1);
	touch.y = ofMap(y, 0, ofGetHeight(), 0, 1);
	touch.z = getTouchZ(ofGetElapsedTimef() - touchDownTime);
	touch.size = 0.01 + touch.z*0.3;
	listener->touchUp(touch);
}
