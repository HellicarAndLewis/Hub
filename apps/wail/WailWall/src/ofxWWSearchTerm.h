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
#include "ofxWWTweet.h"

class ofxWWSearchTerm {
  public:
	ofxWWSearchTerm();
	ofVec2f pos;
	float scale;
	float opacity;
	vector<ofxWWTweet*> associatedTweets;
};