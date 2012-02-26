//
//  Caustics.cpp
//  emptyExample
//
//  Created by Joel Lewis on 23/02/2012.
//  Copyright (c) 2012 Hellicar & Lewis. All rights reserved.
//

#include "Error.h"
#include "Caustics.h"
Caustics::Caustics() {
	ping = NULL;
	pong = NULL;
	verticalDrift = 0.5;
	scaleFactor = 1;
	fade = 0.98;
	brightness = 0.03;
	oscillation = 5;
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
	this->width = width;
	this->height = height;
	ping->allocate(width, height, GL_RGB16);
	pong->allocate(width, height, GL_RGB16);
	triangulator.init(200);
	clearedFbo = false;
	lines.setVertexData(&linePoints[0], linePoints.size(), GL_STREAM_DRAW);
	
//	glGetError();
//	glGenBuffers(1, &lines_vbo); eglGetError();
//	glBindBuffer(GL_ARRAY_BUFFER, lines_vbo); eglGetError();
//	glBufferData(GL_ARRAY_BUFFER, linePoints.size()*sizeof(float), NULL, GL_DYNAMIC_DRAW); eglGetError();
//	glEnableClientState(GL_VERTEX_ARRAY); eglGetError();
}

void Caustics::reset() {
	triangulator.reset();
	indexToPointId.clear();
}
void Caustics::addPoint(const ofVec2f &p, int pointId) {
	int delId = triangulator.addPoint(p.x, p.y);
	//indexToPointId[delId] = pointId;
}

void Caustics::update() {
	triangulator.triangulate();
}

ofVec3f getColor(int a) {
	switch (a%3) {
		case 0: return ofVec3f(1, 0.8, 0.8);
		case 1: return ofVec3f(0.8, 1, 0.8);
		case 2: return ofVec3f(0.8, 0.8, 1);
	}
	
	return ofVec3f();
	
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
		glColor4f(1, 1, 1, fade);
		
		glPushMatrix();
		glTranslatef(width/2, height/2, 0);
		float s = 1 + 0.001 * scaleFactor;
		
		//ofMap(sin(ofGetElapsedTimef()*4), -1, 1, 0.998, 1.002);
		glScalef(s, s, 1);
		pong->setAnchorPercent(0.5, 0.5);
		
		pong->draw(0, verticalDrift);//0.5, ofGetWidth(), ofGetHeight());
		glPopMatrix();
		glColor4f(1, 1, 1, brightness);
		
		int numTris = triangulator.getNumTriangles();
		ITRIANGLE *tris = triangulator.getTriangles();
		XYZ *points = triangulator.getPoints();
		
		currTime = ofGetElapsedTimef();
		
		ofEnableBlendMode(OF_BLENDMODE_ADD);
		linePoints.clear();
		for(int i = 0; i < numTris; i++) {
			ofVec2f a = ofVec2f(points[tris[i].p1].x, points[tris[i].p1].y);
			ofVec2f b = ofVec2f(points[tris[i].p2].x, points[tris[i].p2].y);
			ofVec2f c = ofVec2f(points[tris[i].p3].x, points[tris[i].p3].y);
			//ofVec3f ca = getColor(indexToPointId[tris[i].p1]);
			//ofVec3f cb = getColor(indexToPointId[tris[i].p2]);
			//ofVec3f cc = getColor(indexToPointId[tris[i].p3]);
			drawWavyLine(a, b);
			drawWavyLine(b, c);
			drawWavyLine(c, a);
			

			
			
			
			//ofLine(a, b);
			//ofLine(b, c);
			//ofLine(c, a);
		}
		
//		glBegin(GL_LINES);
//		for(int i = 0; i < linePoints.size(); ++i) {
//			glVertex2fv(&linePoints[i].x);
//		}
//		glEnd();
		
		
//		glBindBuffer(GL_ARRAY_BUFFER, lines_vbo); eglGetError();
		
		glEnableClientState(GL_VERTEX_ARRAY); eglGetError();
		glVertexPointer(2, GL_FLOAT, 0, &linePoints[0].x);
		glDrawArrays(GL_LINES, 0, linePoints.size());
//		glBufferSubData(GL_ARRAY_BUFFER, 0, 

		//lines.updateVertexData(&linePoints[0], linePoints.size());
		//lines.draw(GL_LINES, 0, linePoints.size());
		ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		
	}
	ping->end();
	pong->setAnchorPercent(0, 0);
	ofSetHexColor(0xFFFFFF);
	
}

void Caustics::drawWavyLine(ofVec2f a, ofVec2f b) {
	
	float increment = PI/5.f;
	ofVec2f diff = (b-a);
	float length = diff.length();
	// the normal, normalized
	ofVec2f n = diff/length;
	float ny = n.y;
	n.y = -n.x;
	n.x = ny;
	
	
	
	//step /= PI*2/increment;
	
	//glBegin(GL_LINE_STRIP);
	int i = 0;
	ofVec2f lastPoint;
	//float TWO_PI = PI*2;
	for(float f = 0; f < TWO_PI; f+=increment) {
		float d = f /TWO_PI;//ofMap(f, 0, PI*2, 0, 1);
		
		float window = 1-4*(d-0.5)*(d-0.5);
//		ofVec2f sine = n * sin(f+currTime)*ofMap(length, 0, 200, 0, oscillation)*window;
		ofVec2f sine = n * sin(f+currTime)*length*(oscillation/200)*window;
		ofVec2f p = sine + a + diff * d;
		if(f>0) {
			linePoints.push_back(lastPoint);
			linePoints.push_back(p);
		}
		lastPoint = p;
//		glVertex2f(p.x, p.y);
		i++;
	}
	linePoints.push_back(lastPoint);
	
	linePoints.push_back(b);


	
//	glEnd();
	
	
	
	
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