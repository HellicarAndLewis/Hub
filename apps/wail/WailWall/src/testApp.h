#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxWWScreenManager.h"
#include "ofxWWRenderer.h"
#include "KinectTouchReceiver.h"
#include "KinectTouchSimulator.h"

class testApp : public ofBaseApp, public KinectTouchListener {

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
	
	// touch stuff
	KinectTouchReceiver touchReceiver;
	void touchDown(const KinectTouch &touch);
	void touchMoved(const KinectTouch &touch);
	void touchUp(const KinectTouch &touch);
	KinectTouchSimulator simulator;
	map<int,KinectTouch> blobs;
	
	//one for each triplehead output
	ofxWWScreenManager screenManager;
	ofxWWRenderer renderer;
	
	//screen layout
	string screenSettingsFile;
	bool generateScreens;
	bool shouldLoadScreens;
	bool shouldSaveScreens;
	bool previewScreenLayout;
	
	// screenshots
	GLuint pbo;
	int screen_w;
	int screen_h;
	bool shouldTakeScreenshot;
	string screenshotUsername;
	static void theScreenshotCallback(const string& username, void* user);
	void scaleTouchSize(KinectTouch &touch);
	
	float inMinTouchSize;
	float inMaxTouchSize;
	float outMinTouchSize;
	float outMaxTouchSize;
};
