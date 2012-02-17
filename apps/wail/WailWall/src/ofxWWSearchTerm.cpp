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
	dead = false;
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
	//death attenuation
	if(dead){
		opacity *= ofMap(ofGetElapsedTimef(), killedTime, killedTime+manager->searchTermFadeOutTime, 1.0, 0, true);
	}
	
	searchTermWidth = manager->sharedSearchFont.getStringBoundingBox(term, 0, 0).width;
	pos += force;
	force = ofVec2f(0,0);
}

void ofxWWSearchTerm::draw(){
	
	ofPushStyle();
	ofSetLineWidth(4);
	ofEnableAlphaBlending();
	
	//TEMP USE THIS FOR SEARCH
	ofColor selectedColor = manager->atSignColor;
	ofColor baseColor = manager->layerTwoFontColor;
	baseColor.a = selectedColor.a = opacity*255;
	float holdLerp = isHolding ? ofMap(ofGetElapsedTimef(), holdStartTime, holdStartTime+manager->searchTermMinHoldTime, .0, 1.0, true) : 0.0 ;
	ofSetColor( baseColor.lerp(selectedColor, holdLerp) );
			   
	//TODO center this
	manager->sharedSearchFont.drawString(term, pos.x-searchTermWidth/2, pos.y);
	
	ofPopStyle();
}

void ofxWWSearchTerm::drawDebug(){
	ofPushStyle();
	ofNoFill();
	
	ofSetColor(255, 255, 0);
	ofCircle(pos, manager->searchTermMinDistance);
	
	ofSetColor(255, 0, 0);
	ofCircle(pos, manager->searchTermRepulsionDistance);

	ofSetLineWidth(4);
	ofRect(manager->sharedSearchFont.getStringBoundingBox(term, pos.x-searchTermWidth/2, pos.y) );

	ofFill();
	ofCircle(pos, 10);
	
	ofPopStyle();	
}
