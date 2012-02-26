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
	
	
	
	// tweakable variables
	// how much it goes down by [-2->+2]
	float verticalDrift;
	
	// how much the zoom blur happens [0-10]
	// it doesnt make any difference to performance
	// how much you do.
	float scaleFactor;
	
	
	// how much the back fades out [0.95-1]
	float fade;
	
	
	// [0.01-0.1]
	float brightness;
	
	
	// how much oscillation there is 
	// in the waves. [0-100]?
	// default is 5?
	float oscillation;
	
	float width, height;
private:
	
	void cleanUp();
	
	ofxDelaunay triangulator;
	void drawCaustics();
	
	void pingPong();
	
	ofFbo *ping;
	ofFbo *pong;
	
	ofVbo lines;
	vector<ofVec2f> linePoints;
	
	void drawWavyLine(ofVec2f a, ofVec2f b);

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

