#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate(60);

	for(int i = 0; i < 2; i++){
		fadeBuffer[i].allocate(1024,768, GL_RGBA);	
		fadeBuffer[i].begin();
		ofClear(0);
		fadeBuffer[i].end();
	}
	curbuf = 0;
	
	fadeShader.load("alphafade");
	fadeShader.begin();
	fadeShader.setUniform1i("self",0);
	fadeShader.end();
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0, 0, 0);
	
	fadeBuffer[curbuf].begin();
	ofClear(0);
	ofDisableAlphaBlending();
	ofSetColor(255, 255, 255);	
	fadeShader.begin();
	fadeBuffer[(curbuf+1)%2].draw(0,0);
	fadeShader.end();
	
	ofSetColor(255, 255, 0);
	ofLine(ofGetMouseX(), ofGetMouseY(), ofGetPreviousMouseX(), ofGetPreviousMouseY());
	
	fadeBuffer[curbuf].end();
	
	ofSetColor(0, 255, 0);
	for(int i = -ofGetHeight(); i < ofGetWidth()+ofGetHeight(); i+= 40){
		ofLine(i, 0, i+ofGetHeight(), ofGetHeight());
	}	

	ofEnableAlphaBlending();
	ofSetColor(255, 255, 255);
	fadeBuffer[curbuf].draw(0,0);
	curbuf = (curbuf+1)%2;

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