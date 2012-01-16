#include "testApp.h"
#include "ofxSimpleGuiToo.h"
float waterThreshold = 10;
//--------------------------------------------------------------
void testApp::setup(){
	kinect.init();
	kinect.open();
	depthImg.allocate(640, 480);
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	
	gui.addSlider("Water surface level", waterThreshold, 0, 255);
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
}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetHexColor(0xFFFFFF);
	kinect.drawDepth(0, 0, ofGetWidth(), ofGetHeight());
	if(gui.isOn()) gui.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch(key) {
		case ' ': 
			gui.toggleDraw();
			break;
	}
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