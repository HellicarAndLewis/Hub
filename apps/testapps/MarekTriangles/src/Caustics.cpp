//
//  Caustics.cpp
//  emptyExample
//
//  Created by Joel Lewis on 23/02/2012.
//  Copyright (c) 2012 Hellicar & Lewis. All rights reserved.
//


#include "Caustics.h"
Caustics::Caustics() {
	ping = NULL;
	pong = NULL;
}

Caustics::~Caustics() {
	cleanUp();
}

void Caustics::cleanUp() {
	if(ping!=NULL) {
		delete ping;
		ping = NULL;
	}
	
	if(pong!=NULL) {
		delete pong;
		pong = NULL;
	}
}

void Caustics::setup(int width, int height, int maxNumPoints) {
	cleanUp();
	ping = new ofFbo();
	pong = new ofFbo();
	ping->allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
	pong->allocate(ofGetWidth(), ofGetHeight(), GL_RGB);
	triangulator.init(200);
	clearedFbo = false;
}

void Caustics::reset() {
	triangulator.reset();
}
void Caustics::addPoint(const ofVec2f &p) {
	triangulator.addPoint(p.x, p.y);
}

void Caustics::update() {
	triangulator.triangulate();
}


void Caustics::drawCaustics() {
	if(!clearedFbo) {
		pong->begin();
		ofClear(0, 0, 0, 0);
		pong->end();
		clearedFbo = true;
	}
	ping->begin();
	{
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		ofClear(0, 0, 0, 0);
		//ofEnableBlendMode(OF_BLENDMODE_ADD);
		glColor4f(1, 1, 1, 0.98);
		
		//blur.begin();
		//blur.setUniformTexture("tex", pong->getTextureReference(0), 0);
		glPushMatrix();
		glTranslatef(ofGetWidth()/2, ofGetHeight()/2, 0);
		float s = 1.001;//ofMap(sin(ofGetElapsedTimef()*4), -1, 1, 0.998, 1.002);
		glScalef(s, s, 1);
		pong->setAnchorPercent(0.5, 0.5);
		
		pong->draw(0, 0.5);//0.5, ofGetWidth(), ofGetHeight());
		glPopMatrix();
		//blur.end();
		//		ofRect(0, 0, ofGetWidth(), ofGetHeight());
		glColor4f(1, 1, 1, 0.01);
		
		int numTris = triangulator.getNumTriangles();
		ITRIANGLE *tris = triangulator.getTriangles();
		XYZ *points = triangulator.getPoints();
		
		
		
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		for(int i = 0; i < numTris; i++) {
			ofVec2f a = ofVec2f(points[tris[i].p1].x, points[tris[i].p1].y);
			ofVec2f b = ofVec2f(points[tris[i].p2].x, points[tris[i].p2].y);
			ofVec2f c = ofVec2f(points[tris[i].p3].x, points[tris[i].p3].y);
			ofLine(a, b);
			ofLine(b, c);
			ofLine(c, a);
		}
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		
	}
	ping->end();
	pong->setAnchorPercent(0, 0);
	ofSetHexColor(0xFFFFFF);
}



void Caustics::renderToFbo() {
	
	ofEnableAlphaBlending();
	// push our gl state stuff
	bool lineSmoothedWasOn = glIsEnabled(GL_LINE_SMOOTH);
	if(!lineSmoothedWasOn) {
		glEnable(GL_LINE_SMOOTH);
	}
	float oldLineWidth = 1;
	glGetFloatv(GL_LINE_WIDTH, &oldLineWidth);
	
	glLineWidth(2);
	
	
	
	
	drawCaustics();
	pingPong();
	
	
	
	
	
	
	// pop the gl state stuff
	if(!lineSmoothedWasOn) {
		glDisable(GL_LINE_SMOOTH);
	
	}
	glLineWidth(oldLineWidth);
	
}
ofFbo &Caustics::getFbo() {
	return *pong;
}

void Caustics::pingPong() {
	// do the ping-pong
	ofFbo *temp = ping;
	ping = pong;
	pong = temp;
}