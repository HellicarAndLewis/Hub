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

class ofxWWTweetParticleManager;
class ofxWWSearchTerm {
  public:
	ofxWWSearchTerm();

	void update();
	void draw();
	
	void drawDebug();
	
	ofxWWTweetParticleManager* manager;

	ofVec2f closestPoint;
	int closestTouchID;
	float closestDistanceSquared;
	ofVec2f pos;
	ofVec2f force;
	
	bool wallForceApplied;
	
	bool touchPresent;
	bool isHolding;
	float holdStartTime;
	
	bool selected;
	bool populated;
	
	float scale;
	float opacity;
	
	string user;
	string term;
	vector<ofxWWTweetParticle> associatedTweets;
	
	float searchTermWidth;
	float killedTime;
	bool dead;

};