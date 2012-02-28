/*
 *  ofxWWSearchTerm.h
 *  WailWall
 *
 *  Created by James George on 1/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxWWTweetParticle.h"
#include "FuzzySelection.h"

class ofxWWSearchTermManager;
class ofxWWSearchTerm {
  public:
	ofxWWSearchTerm();

	void update();
	void draw();
	
	void drawDebug();
	
	ofxWWSearchTermManager* manager;

	ofVec2f closestPoint;
	int closestTouchID;
	float closestDistanceSquared;
	ofVec2f pos;
	ofVec2f force;
	
	bool wallForceApplied;
	
	bool touchPresent;
	bool isHolding;
	float holdStartTime;
	

	bool populated;
	
	float scale;
	float opacity;
	int selected_counter;
	
	string user;
	string term;
	vector<ofxWWTweetParticle> associatedTweets;
	
	float searchTermWidth;
	float killedTime;
	bool dead;
	bool took_screenshot; 
	
	// roxlu test
	void highlight();
	void fade();
	
	float selection_started_on;
	bool is_fading;
	bool is_highlighting;
	float faded_on;
	float highlighted_on;
	float tween_duration;
	
	void warmUp();
	void select();
	void deselect();
	float timeOffset;
	float timeSpeedVariation;
	float selectedTime;
private:
	FuzzySelection selection;
};