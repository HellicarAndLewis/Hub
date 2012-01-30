#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxOsc.h"
#include "xmlgui/container/SimpleGui.h"


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
	
	ofxKinect kinect;
	ofxCvGrayscaleImage depthImg;
	ofxOscSender osc;
	
	
	xmlgui::SimpleGui gui;
	
	static const int NUM_MASK_POINTS = 4;
	ofVec2f mask[NUM_MASK_POINTS];
	ofVec2f *dragger;
	void setupMask();
	void saveMask();
};
