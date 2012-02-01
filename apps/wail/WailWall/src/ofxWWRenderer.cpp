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
	
	colorField.loadImage("color_palette.png");
	layer1Opacity = 1.0;

}

void ofxWWRenderer::setupGui(){
	
	gui.addPage("Interaction");
	gui.addSlider("Layer Barrier Z", layerBarrierZ, .25, .75);
	gui.addSlider("Layer Barrier Width", layerBarrierWidth, .25, .75);
	
	gui.addPage("Fluid");
	gui.addSlider("Scale Factor",	fluid.scaleFactor,	1.0, 10.0); 	
	gui.addSlider("OffsetX",		fluid.offsetX,		-200.0, 0); 	
	gui.addSlider("OffsetY",		fluid.offsetY,		-200.0, 0); 	
	gui.addSlider("Force Scale",	fluid.forceScale,	1.0, 200); 
	
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
	renderSecondLayer();
	renderFirstLayer();
	//fluid.draw();
	
	renderTarget.end();	
}

void ofxWWRenderer::renderFirstLayer(){
	float maxTouchZ = 0;
	map<int,KinectTouch>::iterator it;
	for(it = blobs->begin(); it != blobs->end(); it++){
		if(it->second.z > maxTouchZ){
			maxTouchZ = it->second.z;
		}
	}
	float targetOpacity = ofMap(maxTouchZ, layerBarrierZ-layerBarrierWidth/2, layerBarrierZ+layerBarrierWidth/2, 1.0, 0.0, true);
	layer1Opacity += (targetOpacity - layer1Opacity) * .05;
	
	//render the fluid sim for layer 1
	ofPushMatrix();
	ofTranslate(fluid.offsetX, fluid.offsetY);
	ofScale(fluid.scaleFactor, fluid.scaleFactor, 1);
	vector<ofVec2f> verts;
	vector<ofVec2f> texcoords;
	for(int i = 0; i < fluid.numParticles; i++){
		ofxMPMParticle* p = fluid.getParticles()[i];
		verts.push_back(ofVec2f(p->x, p->y));
		verts.push_back(ofVec2f(p->x - p->u, p->y - p->v));
		texcoords.push_back(ofVec2f(p->x, p->y));
		texcoords.push_back(ofVec2f(p->x - p->u, p->y - p->v));
	}
	
	colorField.getTextureReference().bind();
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, &(verts[0].x));
	glTexCoordPointer(2, GL_FLOAT, 0, &(texcoords[0].x));
	glDrawArrays(GL_LINES, 0, verts.size());
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	colorField.getTextureReference().unbind();
	
	tweets.renderTweets();	
}

void ofxWWRenderer::renderSecondLayer(){

}

ofFbo& ofxWWRenderer::getFbo(){
	return renderTarget;
}

void ofxWWRenderer::touchDown(const KinectTouch &touch) {
		
}

void ofxWWRenderer::touchMoved(const KinectTouch &touch) {
	
	//cout << "applying force " << ofVec2f(touch.x, touch.y) << " " << ofVec2f(touch.vel.x, touch.vel.y) << endl;
	
	fluid.applyForce(ofVec2f(touch.x, touch.y), ofVec2f(touch.vel.x, touch.vel.y));
}

void ofxWWRenderer::touchUp(const KinectTouch &touch) {
	
}
