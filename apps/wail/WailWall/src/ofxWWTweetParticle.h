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
	ofxWWTweetParticle();

	ofxWWTweetParticleManager* manager;
	rtt::Tweet tweet;
	
	void setTweet(rtt::Tweet tweet);
	void update();
	void draw();

	//controlled through update
	ofVec2f pos;
	float scale;
	float opacity;	
	float createdTime;
	bool dead;
		
	//calculated on set
	bool isTwoLines;
	float userNameWidth;
	ofRectangle boundingBox;	
	string wordWrappedTweet;
	
};