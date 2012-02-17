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
	void drawDebug();
	
	//controlled through update
	ofVec2f pos;
	ofVec2f lastPos;
	ofVec2f force;
	
	float selectionWeight; //influence from hand
	
	float scale;
	float opacity;	
	float deathAttenuation; //used to fall off wall force
	float createdTime;
	bool dead;
	
	//calculated on set
	bool isTwoLines;

	ofRectangle boundingRect; //updated every frame
	float totalWidth;
	float totalHeight;
	
	float speedAdjust;
	
	bool isSearchTweet;
	bool useBurstOne;

	string lineOne;
	string lineTwo;

	float lineOneWidth;
	float lineOneHeight;
	float lineTwoWidth;
	float lineTwoHeight;
	float userNameWidth;
	float userNameHeight;

	float atSignWidth;
	float atSignHeight;
	
	ofVec2f getBoundingCorner(int cornerIndex); //0-4 top left, top right, bot left, bottom right

  protected:
	ofVec2f getUserDrawPos();
	ofVec2f getTweetLineOneDrawPos();
	ofVec2f getTweetLineTwoDrawPos();
	ofVec2f getAtDrawPos();
	
	void recalculateBoundingRects();

};