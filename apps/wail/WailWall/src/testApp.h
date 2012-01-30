#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxWWScreenManager.h"

class testApp : public ofBaseApp{

public:
	void setup();
	void exit();
	
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	ofxOscReceiver visionOsc;
	

	//one for each triplehead output
	ofxWWScreenManager screenManager;
	
	//screen layout
	string screenSettingsFile;
	bool generateScreens;
	bool shouldLoadScreens;
	bool shouldSaveScreens;
};