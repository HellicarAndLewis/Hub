/*
 *  ofxWWRenderer.h
 *  WailWell
 *
 *  Created by James George on 1/29/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "ofMain.h"
#include "ofxMPMFluid.h"
#include "ofxWWTweetManager.h"

class ofxWWRenderer {
  public:
	
	void setup(int width, int height);
	void setupGui();
	void update();
	void render();
	
	ofFbo& getFbo();
	
  protected:
	ofxWWTweetManager tweets;
	
	void renderFirstLayer();
	void renderSecondLayer();
	
	ofxMPMFluid fluid;
	ofFbo renderTarget;
	
};