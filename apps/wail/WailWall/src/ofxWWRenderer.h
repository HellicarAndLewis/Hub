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
#include "KinectTouchListener.h"

class ofxWWRenderer: public KinectTouchListener {
  public:
	
	void setup(int width, int height);
	void setupGui();
	void update();
	void render();
	
	ofFbo& getFbo();

	// touch stuff
	void touchDown(const KinectTouch &touch);
	void touchMoved(const KinectTouch &touch);
	void touchUp(const KinectTouch &touch);

	
	
  protected:
	ofxWWTweetManager tweets;
	
	void renderFirstLayer();
	void renderSecondLayer();
	
	ofxMPMFluid fluid;
	ofFbo renderTarget;
	
};