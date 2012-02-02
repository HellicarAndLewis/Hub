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
#include "ofxWWTweetManager.h"
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

	//controlled through the guy
	float layerBarrierZ;
	float layerBarrierWidth; //controls layer transition fade	
	
  protected:
	ofxWWTweetManager tweets;
	
	//for diffusing the lines over time
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
	
	//warp distort 
	ofShader warpShader;
	float warpAmount;
	bool justDrawWarpTexture;
	
	void renderLiquidField();
	void renderFirstLayer();
	void renderSecondLayer();
	
	ofVec2f texCoordAtPos(ofImage& image, float x, float y);

	ofxMPMFluid fluid;
	ofFbo firstLayerAccumulator;
	ofFbo liquidTarget;
	ofFbo renderTarget;

	//used as a map into the fluid sim
	ofImage colorField;
	
	float layer1Opacity; //this is smoothed
	
	
};