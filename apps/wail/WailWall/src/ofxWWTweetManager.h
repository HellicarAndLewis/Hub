/*
 *  ofxWWTweetManager.h
 *  WailWall
 *
 *  Created by James George on 1/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"
#include "ofxWWTweet.h"
#include "ofxWWSearchTerm.h"

class ofxWWTweetManager {
  public:
	ofxWWTweetManager();
	void setup();
	void update();
	
  protected:
	vector<ofxWWTweet*> tweets;
	vector<ofxWWSearchTerm*> searchTerms;
};