#pragma once

#include "ofMain.h"
#include "ofxKinect.h"
#include "ofxOpenCv.h"
#include "ofxOsc.h"
#include "xmlgui/container/SimpleGui.h"
#include "ofxCvBlobTracker.h"

class Blob: public ofVec3f {
public:
	static float xySmoothing;
	static float zSmoothing;
	Blob(ofVec3f coords = ofVec3f()) {
		this->x = coords.x;
		this->y = coords.y;
		this->z = coords.z;

	}
	
	void update(ofVec3f coords) {
		this->x = ofLerp(coords.x, this->x, xySmoothing);
		this->y = ofLerp(coords.y, this->y, xySmoothing);
		this->z = ofLerp(coords.z, this->z, zSmoothing);
		
	}
};

class testApp : public ofBaseApp, public ofxCvBlobListener, public xmlgui::Listener {

public:
	void setup();
	void exit();
	
	void update();
	void draw();

	void controlChanged(xmlgui::Control *ctrl);
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
	ofxCvGrayscaleImage rangeScaledImg;
	ofxCvGrayscaleImage maskedImg;
	ofxCvGrayscaleImage bgImg;
	ofxCvContourFinder contourFinder;
	ofxCvBlobTracker blobTracker;
	
	ofxOscSender wallOsc;
	ofxOscSender soundOsc;
	
	
	xmlgui::SimpleGui gui;
	
	static const int NUM_MASK_POINTS = 4;
	ofVec2f mask[NUM_MASK_POINTS];
	ofVec2f *dragger;
	void setupMask();
	void saveMask();
	float waterThreshold;
	float maxWaterDepth;
	float minBlobSize;
	float maxBlobSize;
	
	float blobRotation;
	
	// callbacks for blob listener
	void blobOn( int x, int y, int id, int order );
    void blobMoved( int x, int y, int id, int order );
    void blobOff( int x, int y, int id, int order );

	ofVec3f getBlobCoords(ofxCvTrackedBlob &blob);
	float getBlobSize(ofxCvTrackedBlob &blob);
	void normalizeBlobCoords(ofVec3f &blob);
	map<int,Blob> blobs;
	int viewMode;
	bool drawBlobs;
	float blurSize;
	float blurIterations;
	bool erode;
	bool dilate;
	bool accumulateBackground;
	int backgroundAccumulationCount;
	float backgroundHysteresis;

	
	float lastTimeRotatedBlobs;
};
