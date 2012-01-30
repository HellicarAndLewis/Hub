/*
 *  offxWWScreen.h
 *  WailWell
 *
 *  Created by James George on 1/29/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "ofMain.h"

class ofxWWScreen {
  public:
	
	//in texture space of the renderer FBO
	ofRectangle sourceRect;
	
	//in pixel space of the output canvas
	ofRectangle destRect;
	
	//how is it rotated
	ofOrientation orientation;

	vector<bool> selectedVertices;
	
};

