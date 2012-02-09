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
#include "ofxWWTweetParticleManager.h"
#include "KinectTouchListener.h"

class ofxWWRenderer: public KinectTouchListener {
  public:
	
	void setup(int width, int height);
	void setupGui();
	void update();
	void render();
	
	ofFbo& getFbo();

	// touch stuff
	//set by the testApp
	map<int,KinectTouch>* blobs;
	void touchDown(const KinectTouch &touch);
	void touchMoved(const KinectTouch &touch);
	void touchUp(const KinectTouch &touch);

	//controlled through the gui
	float layerBarrierZ;
	float layerBarrierWidth; //controls layer transition fade	
	bool fakeZOnTouch;
	float fakeZLevel;
	
	ofxWWTweetParticleManager& getTweetManager(); // roxlu 02/07
	
  protected:
	int targetWidth;
	int targetHeight;
	
	//render targers
	ofFbo accumulator;
	ofFbo warpMap;
	ofFbo renderTarget;

	//pass functions
	void typeLayer();
	void renderWarpMap();
	void renderContent();
	
	//sub objects
	ofxMPMFluid fluid;
	ofxWWTweetParticleManager tweets;	
	
	//shader business
	ofShader blurShader;
	float blurAmount;
	float clearSpeed;
	
	//for rendering the layers
	ofShader noiseShader;
	ofImage permutationImage;
	ofVec2f noiseScale;
	
	float noiseFlow;
	float noiseWobbleSpeedX;
	float noiseWobbleSpeedY;
	float noiseWobbleAmplitudeX;
	float noiseWobbleAmplitudeY;
	
	bool enableFluid;
	
	//warp distort 
	ofShader warpShader;
	float warpAmount;
	bool justDrawWarpTexture;
	
	
	ofVec2f texCoordAtPos(ofImage& image, float x, float y);


	//used as a map into the fluid sim
	ofImage colorField;
	
	bool drawTouchDebug;
	
	float layer1Opacity; //this is smoothed
	
	
};