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
	void drawDot();
	void drawText();

	//controlled through update
	ofVec2f pos;
	ofVec2f lastPos;
	ofVec2f force;
	
	float selectionWeight;
	
	float scale;
	float opacity;	
	float deathAttenuation; //used to fall off wall force
	float createdTime;
	bool dead;
	
	//calculated on set
	bool isTwoLines;
	float userNameWidth;
	string wordWrappedTweet;	
	float totalWidth;
	float totalHeight;
	
	bool isSearchTweet;
	
	float lineOneWidth;
	float lineOneHeight;
	float lineTwoWidth;
	string lineOne;
	string lineTwo;

	float atSignWidth;
	float atSignHeight;
	
};