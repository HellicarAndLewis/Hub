#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofBackground(33);
	
	osc.setup("locahost", 4444);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	ofColor(255,255,255);
	ofDrawBitmapString("Press '1' to fake reload bad words\nPress '2' to fake reload hashtag list.", 20,20);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == '1') {
		ofxOscMessage m;
		m.setAddress("/twitter/reload_badwords");
		osc.sendMessage(m);
	}
	else if(key == '2') {
		ofxOscMessage m;
		m.setAddress("/twitter/reload_hashtags");
		osc.sendMessage(m);

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