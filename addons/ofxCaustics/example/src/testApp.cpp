#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(30);

	caustics.setup(512);
}

//--------------------------------------------------------------
void testApp::update(){
	
	//raindrops
	if(ofRandomuf() > .5)
		caustics.addDrop(ofRandomuf()*512, ofRandomuf()*512, 20, ofGetFrameNum() % 2 == 0 ? 0.02 : -0.02);
	
	caustics.light = ofVec3f(ofGetMouseX(), 0, ofGetMouseY());
	caustics.update();
}



//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	caustics.draw(0,0);

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}