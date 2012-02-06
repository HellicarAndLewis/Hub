/*
 *  ofxWWScreenManager.h
 *  WailWell
 *
 *  Created by James George on 1/29/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxWWScreen.h"

class ofxWWScreenManager {
  public:
	ofxWWScreenManager();
		
	void generateScreens(int screensWide, int screensTall, ofOrientation orientation = OF_ORIENTATION_DEFAULT);

	void enableEditing();
	void disableEditing();
	
	void mousePressed(ofMouseEventArgs& args);
	void mouseMoved(ofMouseEventArgs& args);
	void mouseDragged(ofMouseEventArgs& args);
	void mouseReleased(ofMouseEventArgs& args);
	void keyPressed(ofKeyEventArgs& args);
	
	void loadScreens(string xmlFile);
	void saveScreens(string xmlFile);
	
	//assumes the render texture has been bound
	void renderScreens();
	//draw a debug view of how the screens cut up
	void renderLayout();
	
	//get a nicely formatted rectangle for previewing the display texture
	ofRectangle getRenderPreviewRect();
	
	void displayDestinationGui();

	//the source rect is the size of our FBO
	//the destination rect is where it gets rendered, split up to work across multiple displays
	//the preview rect is where it's displayed as a preview for the 1080p output recorder
	ofRectangle sourceRect, destRect, previewRect;	//warning this will override any existing screens
	vector<ofxWWScreen> screens;	
	
  protected:

	bool inited;
	bool editingMode;
	
};