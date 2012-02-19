/*
 *  ofxCaustics.h
 *  example-caustics
 *
 *  Created by James George on 2/19/12.
 *
 */

#pragma once
#include "ofMain.h"

class ofxCaustics {
  public:
	
	ofxCaustics();
	void setup(int textureSize);
	void update();
	
	void addDrop(float x, float y, float radius, float strength);
	void draw(int x, int y);
	
	ofTexture& getTextureRef();
	float delta; //should be between .1 and 1.0, affects propagation speed
	float drag; //should be between about .9 and .999, affects wave slowdown
	ofVec3f light; 
	
  protected:
	ofShader caustics;
	ofShader updater;
	ofShader normals;
	ofShader drop;
	
	int waterswapcur;
	int waterswapnxt;
	
	ofFbo waterTex[2];
	ofFbo causticTex;
	
	void stepSimulation();
	void updateNormals();
	void updateCaustics();
	
	void swapWaterTexture();
};