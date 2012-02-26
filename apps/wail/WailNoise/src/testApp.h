#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "xmlgui/container/SimpleGui.h"

#include "Slosh.h"
#include "AudioSystem.h"
#include "KinectTouchReceiver.h"
#include "OSCInterface.h"
#include "Looper.h"
class testApp : public ofBaseApp, public KinectTouchListener, public xmlgui::Listener {
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
	
	virtual void touchDown(const KinectTouch &touch);
	virtual void touchMoved(const KinectTouch &touch);
	virtual void touchUp(const KinectTouch &touch);
	
	
	ofxOscReceiver soundOsc;
	KinectTouchReceiver kinect;
	
	xmlgui::SimpleGui gui;
	xmlgui::OSCInterface oscInterface;
	
    void controlChanged(xmlgui::Control *ctrl);
	
	ofVec3f rotation;
	map<int,KinectTouch> blobs;
	virtual void audioOut(float *outs, int buffSize, int numChannels);
	Slosh slosh;
	Slosh splash;
	
    float bubbleVolume;
    float bgLoopVolume;
	float splashVolume;
	audio::PlayerRef bgLoopPlayer;
	audio::EffectRef hiPass;
	audio::EffectRef reverb;
	
	audio::BusRef reverbBus;
	void chasePan(float pan);
	void setPan(float pan);
	audio::PlayerRef lows[3];
	audio::PlayerRef highs[3];
	void chaseDepth(float z);
	void chaseDisturbance(float dxy);
	void setLowVolume(float vol);
	void setHighVolume(float vol);
	Looper lowLooper;
};

