/*
 *  ofxWWRenderer.cpp
 *  WailWell
 *
 *  Created by James George on 1/29/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxWWRenderer.h"
#include "ofxSimpleGuiToo.h"

void ofxWWRenderer::setup(int width, int height){
	renderTarget.allocate(width, height, GL_RGB, 4);
	fluid.setup(100000);
	fluid.scaleFactor = 6.4;
}

void ofxWWRenderer::setupGui(){
	
	gui.addPage("Fluid");
	gui.addSlider("Particles",		fluid.numParticles,		1000, 100000); 
	gui.addSlider("Density",		fluid.densitySetting,	0, 30.0);	
	gui.addSlider("Stiffness",		fluid.stiffness,		0, 2.0);
	gui.addSlider("Bulk Viscosity",	fluid.bulkViscosity,	0, 10.0);
	gui.addSlider("Elasticity",		fluid.elasticity,		0, 4.0);
	gui.addSlider("Viscosity",		fluid.viscosity,		0, 4.0);
	gui.addSlider("Yield Rate",		fluid.yieldRate,		0, 2.0);
	gui.addSlider("Gravity",		fluid.gravity,			0, 0.02);
	gui.addSlider("Smoothing",		fluid.smoothing,		0, 3.0);
	gui.addToggle("Do Obstacles",	fluid.bDoObstacles); 	
}

void ofxWWRenderer::update(){
	fluid.update();
}

void ofxWWRenderer::render(){
	
	renderTarget.begin();
	
	ofClear(0);
	fluid.draw();
	
	renderTarget.end();	
}

ofFbo& ofxWWRenderer::getFbo(){
	return renderTarget;
}

void ofxWWRenderer::touchDown(const KinectTouch &touch) {
	
}
void ofxWWRenderer::touchMoved(const KinectTouch &touch) {
	
}
void ofxWWRenderer::touchUp(const KinectTouch &touch) {
	
}