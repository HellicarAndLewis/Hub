#include "testApp.h"

//
//
// NOTE FROM MAREK - 
// 
// I made a new project called "WailWall" which is for the visualization.
//
// This project is just for the kinect reading, and will have OSC going out
// to the "WailWall" app.
//

float waterThreshold = 10;	
//--------------------------------------------------------------
void testApp::setup(){
	kinect.init();
	kinect.open();
	kinect.setUseTexture(false);
	depthImg.allocate(kinect.getWidth(), kinect.getHeight());
	
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	
	
	gui.addSlider("Water Surface Level", waterThreshold, 0, 255);
	

	gui.loadFromXML();
	gui.setAutoSave(true);
	
}

void testApp::exit() {
	kinect.close();
}
//--------------------------------------------------------------
void testApp::update(){
	kinect.update();
	if(kinect.isFrameNew()) {
		depthImg.setFromPixels(kinect.getDepthPixels(),640,480);
		
	}
	
	if(generateScreens){
		generateScreens = false;
		screenManager.generateScreens(5, 3);
		screenManager.saveScreens(screenSettingsFile);
	}
	
	if(shouldLoadScreens){
		shouldLoadScreens = false;
		screenManager.loadScreens(screenSettingsFile);
	}
	
	if(shouldSaveScreens){
		shouldSaveScreens = false;
		screenManager.saveScreens(screenSettingsFile);
	}	
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	
//	ofSetHexColor(0xFFFFFF);
	kinect.drawDepth(0, 0, ofGetWidth(), ofGetHeight());
	

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