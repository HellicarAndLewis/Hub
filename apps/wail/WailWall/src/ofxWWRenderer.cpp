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
	renderTarget.allocate(width, height, GL_RGB);
	firstLayerAccumulator.allocate(width, height, GL_RGBA);
	liquidTarget.allocate(width, height, GL_RGB);
	
	firstLayerAccumulator.begin();
	ofClear(0);
	firstLayerAccumulator.end();
	
	fluid.setup(100000);
	fluid.scaleFactor = 6.4;
	
	colorField.loadImage("color_palette.png");
	layer1Opacity = 1.0;
	
	permutationImage.loadImage("shaders/permtexture.png");
	noiseShader.load("shaders/noise");
	noiseShader.begin();
	noiseShader.setUniform1f("permTexture", 0);
	noiseShader.end();

}

void ofxWWRenderer::setupGui(){
	
	gui.addPage("Interaction");
	gui.addSlider("Layer Barrier Z", layerBarrierZ, .25, .75);
	gui.addSlider("Layer Barrier Width", layerBarrierWidth, 0.05, .25);
	
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

	renderLiquidField();
	renderFirstLayer();
	renderSecondLayer();
	
	renderTarget.begin();
	ofClear(0);
	ofEnableAlphaBlending();
	
	liquidTarget.draw(0,0);
	firstLayerAccumulator.draw(0,0);
	tweets.renderTweets();	
		
	renderTarget.end();	
}

void ofxWWRenderer::renderFirstLayer(){
	
	firstLayerAccumulator.begin();
	ofPushStyle();
	ofSetColor(255, 255, 255);
	
	ofEnableAlphaBlending();
	
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
	vector<ofVec3f> colors;
	for(int i = 0; i < fluid.numParticles; i++){
		ofxMPMParticle* p = fluid.getParticles()[i];
		verts.push_back(ofVec2f(p->x, p->y));
		verts.push_back(ofVec2f(p->x - p->u, p->y - p->v));
		texcoords.push_back( texCoordAtPos(colorField, p->x, p->y) );
		texcoords.push_back( texCoordAtPos(colorField, p->x - p->u, p->y - p->v) );
	}
	
	colorField.getTextureReference().bind();
	
//	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, &(verts[0].x));
	glTexCoordPointer(2, GL_FLOAT, 0, &(texcoords[0].x));
	glDrawArrays(GL_LINES, 0, verts.size());
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
//	glDisableClientState(GL_COLOR_ARRAY);

	colorField.getTextureReference().unbind();

	ofSetColor(0,0,0, 10);
	ofRect(0, 0, firstLayerAccumulator.getWidth(), firstLayerAccumulator.getHeight());
	
	ofPopStyle();
	firstLayerAccumulator.end();
}


void ofxWWRenderer::renderLiquidField(){
	
	liquidTarget.begin();
	ofClear(0);
	
	noiseShader.begin();
	
	noiseShader.setUniform1f("flow", ofGetElapsedTimef()*100);
	noiseShader.setUniform1f("wobble1", sin(ofGetElapsedTimef()/2.0)*5);
	noiseShader.setUniform1f("wobble2", sin(ofGetElapsedTimef()/20.0)*5);
	noiseShader.setUniform2f("scale", 500.0, 200.0);
	
	permutationImage.getTextureReference().bind();
	
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	
	glTexCoord2f(liquidTarget.getWidth(), 0);
	glVertex2f(liquidTarget.getWidth(), 0);

	glTexCoord2f(liquidTarget.getWidth(), liquidTarget.getHeight());
	glVertex2f(liquidTarget.getWidth(), liquidTarget.getHeight());

	glTexCoord2f(0, liquidTarget.getHeight());
	glVertex2f(0, liquidTarget.getHeight());

	permutationImage.getTextureReference().unbind();
	glEnd();
	
	noiseShader.end();
	
	liquidTarget.end();
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

ofVec2f ofxWWRenderer::texCoordAtPos(ofImage& image, float x, float y){
	return ofVec2f(ofMap(x, 0, fluid.getGridSizeX(), 0, image.getWidth()),
				   ofMap(y, 0, fluid.getGridSizeY(), 0, image.getHeight()));
}
