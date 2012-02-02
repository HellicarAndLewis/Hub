/*
 *  ofxWWTweet.h
 *  WailWall
 *
 *  Created by James George on 1/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"
#include "TwitterApp.h"

class ofxWWTweetParticle {
  public:
	//copies the data we 
	ofxWWTweetParticle();
	
	void setTweet(rtt::Tweet& tweet);
	void update();
	
	string text;
	
	ofVec2f pos;
	float scale;
	float opacity;	
	
	
	rtt::Tweet tweet;
};