#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(33);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
	if(!twitter_app.initDB()){
		printf("Error: cannot initialize twitter db.\n");
		exit();
	}
	
	twitter_app.addDefaultListener();
	
	// What do you want to track?
	/*
	twitter_app.track("ItsFunnyHow");
	twitter_app.track("NeverTrustAGuyWho");
	twitter_app.track("TolimaDay");
	twitter_app.track("love");
*/
	//twitter_app.track("love");
	if(!twitter_app.connect()) {
		printf("Error: cannot connect to twitter stream.\n");
		exit();
	}
	
	twitter_app.addListener(this, &testApp::onNewSearchTerm);
	uploader.startThread();
	take_photo = false;
}


void testApp::onNewSearchTerm(TwitterAppEvent& event) {
	//printf("Yay new search term: %s\n", event.search_term.c_str());
}

//--------------------------------------------------------------
void testApp::update(){
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
	twitter_app.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	float p = (1.0f + (cos(ofGetElapsedTimef()*2.6) + sin(ofGetElapsedTimef()* 0.5)) * 0.5);
	float s = 10 + p * 100;
	ofSetColor(244,p*155,255-p*p*100);
	ofCircle(ofGetWidth() * 0.5,ofGetHeight()*0.5,s);
	
	if(take_photo) {
		string filename = ofGetTimestampString() +".jpg";
		ofSaveScreen(filename);
		//for(int i = 0; i < 5; ++i) {
			uploader.addFile(ofToDataPath(filename,true), "roxlu", "@dewarscube SEARCH biology");
			printf("created : %s\n", filename.c_str());
		//}
		take_photo = false;
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	if(key == ' ') {
		take_photo = !take_photo;
	}
	
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