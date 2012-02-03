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
	highlighted = false;
	manager = NULL;
}

void ofxWWSearchTerm::update(){
	if(!touchPresent){
		opacity = 0;
		highlighted = false;
		selected = false;
	}
	else if(selected){
		opacity = 1.0;
	}
	// there is a touch && we aren't yet selected, calculate th new opacity
	else{
		opacity = 1 - manager->tweetLayerOpacity;
		float distance = closestPoint.distance(pos);
//		if(term == "OPENFRAMEWORKS"){
//			cout << "closest distance is " << distance << endl;
//		}
		
		if(!highlighted && distance < manager->searchTermMinDistance){
			highlighted = true;
			holdStartTime = ofGetElapsedTimef();
		}
		else if(highlighted && distance > manager->searchTermMinDistance){
			highlighted = false;
		}
		
		if(highlighted && (ofGetElapsedTimef() - holdStartTime) > manager->searchTermMinHoldTime){
			selected = true;
		} 		
	}
}

void ofxWWSearchTerm::draw(){
	
	ofPushStyle();
	ofEnableAlphaBlending();
	ofColor selectedColor = ofColor::fromHex(0xe6ab38, opacity*255);
	ofColor baseColor = ofColor(255,255,255,opacity*255);
	float holdLerp = highlighted ? ofMap(ofGetElapsedTimef(), holdStartTime, holdStartTime+manager->searchTermMinHoldTime, .0, 1.0, true) : 0.0 ;
	ofSetColor( baseColor.lerp(selectedColor, holdLerp) );
			   
	manager->sharedLargeFont.drawString(term, pos.x, pos.y);
	
	ofPopStyle();
}