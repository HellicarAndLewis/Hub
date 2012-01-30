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

class ofxWWTweet{
  public:
	ofxWWTweet();
	void update();
	
	ofVec2f pos;
	float scale;
	float opacity;	
};