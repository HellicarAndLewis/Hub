#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	if(!twitter_app.initDB()){
		printf("Error: cannot initialize twitter db.\n");
		exit();
	}
	
	twitter_app.addDefaultListener();
	
	// What do you want to track?
	twitter_app.track("ItsFunnyHow");
	twitter_app.track("NeverTrustAGuyWho");
	twitter_app.track("TolimaDay");
	twitter_app.track("love");

	if(!twitter_app.connect()) {
		printf("Error: cannot connect to twitter stream.\n");
		exit();
	}
	
}

//--------------------------------------------------------------
void testApp::update(){
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
	twitter_app.update();
}

//--------------------------------------------------------------
void testApp::draw(){

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