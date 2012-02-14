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
	ofxWWTweetParticleManager* manager;

	ofVec2f closestPoint;
	ofVec2f pos;
	
	//weight contributed by all surrounding touches
	float touchWeight;

	bool touchPresent;
	bool highlighted;
	float holdStartTime;
	
	bool selected;
	bool populated;
	
	float scale;
	float opacity;
	
	string user;
	string term;
	vector<ofxWWTweetParticle> associatedTweets;
};