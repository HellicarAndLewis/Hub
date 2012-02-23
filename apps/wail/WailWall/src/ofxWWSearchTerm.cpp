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
#include "mathutils.h"

ofxWWSearchTerm::ofxWWSearchTerm(){
	selectedTime = -1000;
	isHolding = false;
	manager = NULL;
	dead = false;
	opacity  = 0.5;
	selected_counter = 0;
	took_screenshot = false;
	
	is_fading = false;
	is_highlighting = false;
	tween_duration = 500;
	timeOffset = ofRandom(0, 500);
	timeSpeedVariation = ofRandom(0.5, 1.5);
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
	
	
	// this does the floaty
	float t = timeSpeedVariation * ofGetElapsedTimef()+timeOffset;
	float dy = sin(t/6.f) * manager->movement;
	float dx = sin(t/7.f) * manager->movement;

	// add the floaty in only if we're not selected.
	pos.x += dx*(1-selection.getValue());
	pos.y += dy*(1-selection.getValue());
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
	
	
	float t = timeSpeedVariation * ofGetElapsedTimef()+timeOffset;
	
	//TODO center this
	
	// give the search term a floaty movement
	float scaleFactor = ofMap(sin(t/4.f), -1, 1, 0.9, 1.1);
	
	

	ofColor c = baseColor.lerp(selectedColor, p);
	
	// make the colour brighter if the scale is bigger
	c.a *= ofMap(scaleFactor, 0.9, 1.2, 0.6, 1);
	float pulseDuration = 1.5;
	if(ofGetElapsedTimef()-selectedTime<pulseDuration) {
		
		float tt = ofGetElapsedTimef()-selectedTime;
		float fastStart = 0.1;
		if(tt<fastStart*pulseDuration) {
			tt = ofMap(tt, 0, fastStart*pulseDuration, 0, 0.5*pulseDuration);
		} else {
			tt = ofMap(tt, fastStart*pulseDuration, pulseDuration, 0.5*pulseDuration, pulseDuration);
		}
		float x = ofMap(tt, 0, pulseDuration, 0, 1, true);
		float pulseScale = (1-cos(x*2*PI))/2;
		scaleFactor += pulseScale * 0.5;
	}
	ofSetColor( c );
	glPushMatrix();
	{
		glTranslatef(pos.x, pos.y, 0);
		glScalef(scaleFactor, scaleFactor, 1);
		glTranslatef(-searchTermWidth/2, 0, 0);
		
		manager->parent->sharedSearchFont.drawString(term, 0, 0);
	}
	glPopMatrix();
	
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
	selectedTime = ofGetElapsedTimef();
}
void ofxWWSearchTerm::deselect() {
	selection.setTarget(0);
}