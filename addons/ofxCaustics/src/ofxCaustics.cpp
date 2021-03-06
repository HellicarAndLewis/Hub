/*
 *  ofxCaustics.cpp
 *  example-caustics
 *
 *  Created by James George on 2/19/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxCaustics.h"
ofxCaustics::ofxCaustics(){
	light = ofVec3f(0,1,1).normalized();
	delta = 1.0;	
	drag = .95;
	shaderPath = "";
}

void ofxCaustics::setup(int textureWidth, int textureHeight){
	drop.load(shaderPath+"dropshader");
	updater.load(shaderPath+"updateshader");
	normals.load(shaderPath+"updatenormals");
	caustics.load(shaderPath+"caustics");
	
	caustics.begin();
	caustics.setUniform1f("water", 0);
	caustics.end();
	
	updater.begin();
	updater.setUniform1f("texture", 0);
	updater.end();
	
	normals.begin();
	normals.setUniform1f("texture", 0);
	normals.end();
	
	drop.begin();
	drop.setUniform1f("texture", 0);
	drop.end();
	
	causticTex.allocate(textureWidth, textureHeight, GL_RGBA32F_ARB);
	causticTex.begin();
	ofClear(0);
	causticTex.end();
	
	for(int i = 0; i < 2; i++){
		waterTex[i].allocate(textureWidth, textureHeight, GL_RGBA32F_ARB);
		waterTex[i].begin();
		ofClear(0,0,0,0);
		waterTex[i].end();
	}
	
	waterswapcur = 0;
	waterswapnxt = 1;	
}

void ofxCaustics::update(){
	stepSimulation();
	stepSimulation();
	updateNormals();
	updateCaustics();		
}

void ofxCaustics::addDrop(float x, float y, float radius, float strength){
	
	waterTex[waterswapcur].begin();
	
	drop.begin();
	drop.setUniform2f("center", x, y);
	drop.setUniform1f("radius", radius);
	drop.setUniform1f("strength", strength);
	
	waterTex[waterswapnxt].draw(0, 0);
	
	drop.end();
	
	waterTex[waterswapcur].end();
	
	swapWaterTexture();	
}

void ofxCaustics::draw(int x, int y){
	waterTex[waterswapcur].draw(0,0);
	causticTex.draw(waterTex[waterswapcur].getWidth(),0);
}

ofTexture& ofxCaustics::getTextureReference(){
	return causticTex.getTextureReference();
}

void ofxCaustics::stepSimulation(){
	waterTex[waterswapcur].begin();
	ofDisableAlphaBlending();
	
	updater.begin();
	updater.setUniform2f("delta",delta,delta);
	updater.setUniform1f("drag",drag);
	waterTex[waterswapnxt].draw(0, 0);
	
	updater.end();
	
	waterTex[waterswapcur].end();
	
	swapWaterTexture();		
}

void ofxCaustics::updateNormals(){
	waterTex[waterswapcur].begin();
	ofDisableAlphaBlending();
	
	normals.begin();
	normals.setUniform2f("delta",delta,delta);
	
	waterTex[waterswapnxt].draw(0, 0);
	
	normals.end();
	
	waterTex[waterswapcur].end();
	
	swapWaterTexture();		
}

void ofxCaustics::updateCaustics(){
	
	causticTex.begin();
	ofClear(0);
	
	caustics.begin();
	ofVec3f unit = light.normalized();
	caustics.setUniform3f("light", unit.x, unit.y, unit.z);
	
	waterTex[waterswapcur].draw(0, 0);
	caustics.end();
	
	causticTex.end();
}

void ofxCaustics::swapWaterTexture(){
	waterswapcur = (waterswapcur + 1) % 2;
	waterswapnxt = (waterswapnxt + 1) % 2;
}
