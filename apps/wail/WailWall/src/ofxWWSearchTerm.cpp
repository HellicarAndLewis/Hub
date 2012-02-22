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
#include "Colours.h"

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
	
	selection.update();
	if(!touchPresent){
		isHolding = false;
	}
	float minAlpha = ofMap(selection.getValue(), 0, 1, manager->searchTermMinAlpha, manager->selectedTermMinAlpha);
	opacity = ofMap(manager->parent->tweetLayerOpacity, 1, 0, minAlpha, 1);
	
	//death attenuation
	if(dead){
		opacity *= ofMap(ofGetElapsedTimef(), killedTime, killedTime+manager->fadeOutTime, 1.0, 0, true);
	}
	
	searchTermWidth = manager->parent->sharedSearchFont.getStringBoundingBox(term, 0, 0).width;
	pos += force;
	force = ofVec2f(0,0);
}


void ofxWWSearchTerm::draw(){

	ofPushStyle();
	ofSetLineWidth(4);
	ofEnableAlphaBlending();
	
	//TEMP USE THIS FOR SEARCH
	ofColor selectedColor = ofColor::fromHex(Colours::get(AT_SIGN));
	ofColor baseColor = ofColor::fromHex(Colours::get(LAYER_2_FONT));
	
	baseColor.a = selectedColor.a = opacity*255;
	float holdLerp = 1.0;

	
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
	
	p = selection.getValue();
	ofSetColor( baseColor.lerp(selectedColor, p) );
	
	//TODO center this
	manager->parent->sharedSearchFont.drawString(term, pos.x-searchTermWidth/2, pos.y);
	
	ofPopStyle();
}

void ofxWWSearchTerm::drawDebug(){
	ofPushStyle();
	ofNoFill();
	
	
	ofSetColor(255, 0, 0);
	ofCircle(pos, manager->repulsionDistance);

	ofSetLineWidth(4);
	ofRect(manager->parent->sharedSearchFont.getStringBoundingBox(term, pos.x-searchTermWidth/2, pos.y) );

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

void ofxWWSearchTerm::warmUp() {
	selection.warmUp();
	printf("warm up\n");
}
void ofxWWSearchTerm::select() {
	selection.setTarget(1);
}
void ofxWWSearchTerm::deselect() {
	selection.setTarget(0);
}