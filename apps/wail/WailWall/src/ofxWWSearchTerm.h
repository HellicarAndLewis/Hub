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

	void draw();
	ofxWWTweetParticleManager* manager;

	ofVec2f pos;
	float scale;
	float opacity;
	bool tweetsGenerated;
	string term;
	vector<ofxWWTweetParticle> associatedTweets;
};