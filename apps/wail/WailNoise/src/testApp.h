#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "xmlgui/container/SimpleGui.h"

#include "Slosh.h"
#include "AudioSystem.h"
#include "KinectTouchReceiver.h"

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
	
	virtual void touchDown(int id, ofVec3f touch);
	virtual void touchMoved(int id, ofVec3f touch);
	virtual void touchUp(int id);
	
	
	ofxOscReceiver soundOsc;
	KinectTouchReceiver kinect;
	
	xmlgui::SimpleGui gui;
	
	map<int,ofVec3f> blobs;
	virtual void audioOut(float *outs, int buffSize, int numChannels);
	Slosh slosh;
	Slosh drop;
	audio::PlayerRef bgLoopPlayer;
};

