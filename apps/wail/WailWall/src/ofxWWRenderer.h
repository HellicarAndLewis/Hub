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
#include "ofxCaustics.h"
#include "CallToAction.h"

class ofxWWRenderer: public KinectTouchListener {
  public:
	
	void setup(int width, int height);
	void setupGui();
	void update();
	void render();
	
	ofFbo& getFbo();
	ofFbo& getScreenshotFbo();

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
	ofxWWSearchTermManager& getSearchTermManager();
	
	void stopFluidThread();
	
	void keyPressed(ofKeyEventArgs& args); //roxlu: testing screenshots
	bool test_screenshot; // roxlu: testing screenshots
	
  protected:
	int targetWidth;
	int targetHeight;
	

	//render targers
	int accumbuf;
	ofFbo accumulator[2];
	ofFbo warpMap;
	ofFbo renderTarget;
	ofFbo gradientOverlay;
	ofFbo layer1Target;
	ofFbo layer2Target;
	ofFbo screenshotTarget;
    ofFbo background;
	
	//pass functions
	void typeLayer();
	void renderWarpMap();
	void renderDynamics();
	void renderGradientOverlay();
    void renderBackground();
	
	void renderLayer1();
	void renderLayer2();
	
	ofxCaustics caustics;
	bool enableCaustics;
	bool drawCausticsDebug;
	float dropScale;
	float dropForce;
	
	//sub objects
	ofxMPMFluid fluid;
	ofxWWTweetParticleManager tweets;	
	
	//for rendering connections
	ofShader alphaFade;
	bool enableConnections;
	float fadeSpeed;
	
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

	//combine the caustics together
	ofShader glowShader;
	float glowAmount;
	
	//used as a map into the fluid sim
	bool useBackgroundSetA;
	ofImage colorField;
	ofImage layerOneBackgroundA;
	ofImage layerTwoBackgroundA;
	ofImage layerOneBackgroundB;
	ofImage layerTwoBackgroundB;
	
	ofImage halo;
	
	bool drawTouchDebug;
	
	float layer1Opacity; //this is smoothed
	
	ofVec2f randomPointInCircle(ofVec2f position, float radius);
	
	
	// this takes care of call-to-action
	// timing and drawing
	CallToAction callToAction;

    int haloSurfaceColourHex;
    int haloBottomColourHex;
    
    int surfaceColourHex;
    int bottomColourHex;

    float smootherStep(float edge0, float edge1, float x); //from http://en.wikipedia.org/wiki/Smoothstep
};

inline ofFbo& ofxWWRenderer::getScreenshotFbo() {
	return screenshotTarget;
}
