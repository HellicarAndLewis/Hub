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
	//for this to be accureate, preview rect needs to be the same aspect ratio as the actual destination rectangle
	void renderPreview();
	
	void displayDestinationGui();
	
	ofRectangle sourceRect, destRect;	//warning this will override any existing screens
	vector<ofxWWScreen> screens;	
	
  protected:

	bool inited;
	bool editingMode;
	
};