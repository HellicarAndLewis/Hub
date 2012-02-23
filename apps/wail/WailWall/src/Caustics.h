//
//  Caustics.h
//  emptyExample
//
//  Created by Joel Lewis on 23/02/2012.
//  Copyright (c) 2012 Hellicar & Lewis. All rights reserved.
//

#pragma

#include "ofMain.h"
#include "ofxDelaunay.h"

class Caustics {
public:
	void setup(int width, int height, int maxNumPoints);
	
	

	// every frame, call reset(), add your points, then update()
	
	void reset();
	void addPoint(const ofVec2f &p, int pointId);
	// generates delaunay
	void update();

	void renderToFbo();
	
	
	// this won't be the same fbo
	// every time, it's a pingpong.
	ofFbo &getFbo();
	
	
	Caustics();
	~Caustics();
	
	float width, height;
private:
	
	void cleanUp();
	
	ofxDelaunay triangulator;
	void drawCaustics();
	
	void pingPong();
	
	ofFbo *ping;
	ofFbo *pong;
	
	void drawLine(ofVec2f a, ofVec2f b, ofVec3f ca, ofVec3f cb);

	float currTime;
	// check if the FBO has been cleared for initialization
	bool clearedFbo;
	
	struct PhaseInfo {
		float phase;
		bool visited;
		PhaseInfo() {
			phase = 0;
			visited = false;
		}
	};
	map<int,int> indexToPointId;
	map<pair<int, int>, PhaseInfo> phases;
};

