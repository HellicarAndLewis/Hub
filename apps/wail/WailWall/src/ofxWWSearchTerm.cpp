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
	dead = false;
	opacity  = 0.5;
	selected_counter = 0;
	took_screenshot = false;
	
	is_fading = false;
	is_highlighting = false;
	tween_duration = 500;
}

void ofxWWSearchTerm::update(){
	
	if(!touchPresent){
		isHolding = false;
	}
	/*
	float targetOpacity;	
	if(selected){
		targetOpacity = 1.0;
		opacity = 1.0;
	}
	else {
		targetOpacity = (1 - manager->tweetLayerOpacity)+manager->searchMinOpacity;
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
	
<<<<<<< HEAD
	opacity += (targetOpacity - opacity)*.1;
	//printf("opacity: %f\n", opacity);
=======
	*/
	
	
	
	//opacity += (targetOpacity - opacity)*.1;
//>>>>>>> origin/flowchange
	//opacity = targetOpacity;
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
	float holdLerp = 0.0;
	// opacity = 0.5;
	//if(selected){
	//	opacity = 1;
		holdLerp = 1.0;
	//}
	//opacity = ofMap(selected_counter, 0, 60, 0.5, 1, true);
	//holdLerp = ofMap(selected_counter, 0, 60, 0, 1, true);
	/*else if(isHolding){
		holdLerp = ofMap(ofGetElapsedTimef(), holdStartTime, holdStartTime+manager->searchTermMinHoldTime, .0, 1.0, true);
	}*/
	
	float p = 0.0; 
	if(is_highlighting) {
		
		float now = ofGetElapsedTimeMillis();
		float diff =  tween_duration-(highlighted_on - now);
		p = MIN(1,diff / tween_duration);
	}
	else if(is_fading) {
		float now = ofGetElapsedTimeMillis();
		float diff =  tween_duration-(faded_on - now);
		p = 1.0 - MIN(1,diff / tween_duration);
	}
	
	ofSetColor( baseColor.lerp(selectedColor, p) );
			   
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



void ofxWWSearchTerm::fade() {
	highlighted_on = 0;
	is_highlighting = false;
	if(!is_fading) {
		faded_on = ofGetElapsedTimeMillis() + tween_duration;
		is_fading = true;
	}
}

void ofxWWSearchTerm::highlight() {	
	faded_on = 0;
	is_fading = false;
	if(!is_highlighting) {
		highlighted_on = ofGetElapsedTimeMillis() + tween_duration;
		is_highlighting = true;
	}
}
