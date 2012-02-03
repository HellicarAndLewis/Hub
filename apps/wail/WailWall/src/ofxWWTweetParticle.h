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


class ofxWWTweetParticleManager;
class ofxWWTweetParticle {
  public:
	ofxWWTweetParticleManager* manager;
	
	//copies the data we 
	ofxWWTweetParticle();
	
	void setTweet(rtt::Tweet tweet);
	void update();
	void draw();

	float wordWrapLength;

	ofRectangle boundingBox;
	ofVec2f pos;
	float scale;
	float opacity;	
	float createdTime;
	bool dead;
		
//	string lineOne;
//	string lineTwo;
	bool isTwoLines;
	string wordWrappedTweet;
	rtt::Tweet tweet;
};