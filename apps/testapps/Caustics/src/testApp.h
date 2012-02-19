#pragma once

#include "ofMain.h"

class testApp : public ofBaseApp{

  public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	ofShader caustics;
	ofShader updater;
	ofShader normals;
	ofShader drop;
	
	int waterswapcur;
	int waterswapnxt;
	
	ofFbo waterTex[2];
	ofFbo causticTex;
	
	void addDrop(float x, float y, float radius, float strength);
	void stepSimulation();
	void updateNormals();
	void updateCaustics();
	
	void swapWaterTexture();
};
