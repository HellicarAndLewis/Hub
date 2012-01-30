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

class ofxWWRenderer {
  public:
	void setup(int width, int height);
	void update();
	void draw();
  protected:
	ofFbo renderTarget;
	
};