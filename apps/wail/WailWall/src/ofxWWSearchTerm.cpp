/*
 *  ofxWWSearchTerm.cpp
 *  WailWall
 *
 *  Created by James George on 1/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxWWSearchTerm.h"
#include "ofxWWTweetParticleManager.h"

ofxWWSearchTerm::ofxWWSearchTerm(){
	selected = false;
	isHolding = false;
	manager = NULL;
	touchWeight = 0;
}

void ofxWWSearchTerm::update(){
	float targetOpacity;
	if(!touchPresent){
		targetOpacity = manager->searchMinOpacity;
		isHolding = false;
		selected = false;
	}
	else if(selected){
		targetOpacity = 1.0;
	}
	// there is a touch && we aren't yet selected, calculate th new opacity
	else {
		targetOpacity = 1 - manager->tweetLayerOpacity;
		float distance = closestPoint.distance(pos);
		
		if(!isHolding && distance < manager->searchTermMinDistance){
			isHolding = true;
			holdStartTime = ofGetElapsedTimef();
		}
		else if(isHolding && distance > manager->searchTermMinDistance){
			isHolding = false;
		}
		
		if(isHolding && (ofGetElapsedTimef() - holdStartTime) > manager->searchTermMinHoldTime){
			selected = true;
		} 		
	}
	
	opacity += (targetOpacity - opacity)*.1;
	pos += force;
	force = ofVec2f(0,0);
}

void ofxWWSearchTerm::draw(){
	
	ofPushStyle();
	ofEnableAlphaBlending();
//	opacity = 1.0;
	ofColor selectedColor = ofColor::fromHex(0xe6ab38, opacity*255);
	ofColor baseColor = ofColor(255,255,255,opacity*255);
	float holdLerp = isHolding ? ofMap(ofGetElapsedTimef(), holdStartTime, holdStartTime+manager->searchTermMinHoldTime, .0, 1.0, true) : 0.0 ;
	ofSetColor( baseColor.lerp(selectedColor, holdLerp) );
			   
	manager->sharedSearchFont.drawString(term, pos.x, pos.y);
	
	ofPopStyle();
}

void ofxWWSearchTerm::drawDebug(){
	ofPushStyle();
	ofNoFill();
	
	ofSetColor(255);
	ofCircle(pos, manager->searchTermMinDistance);
	ofFill();
	ofCircle(pos, 10);
	
	ofPopStyle();	
}
