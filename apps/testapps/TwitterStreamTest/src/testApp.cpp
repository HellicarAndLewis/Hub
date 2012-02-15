#include "testApp.h"

testApp::testApp()
	:stream(twitter)
{
}

void testApp::setup(){
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofBackground(33);
	
	// small test with search terms.
	terms.setup(ofToDataPath("searches.bin",true));
	terms.addSearchTerm("roxlu", "some search term");
	terms.addSearchTerm("james", "something else");
	terms.save();
	terms.load();
	/*
	twitter.setConsumerKey("kyw8bCAWKbkP6e1HMMdAvw");
	twitter.setConsumerSecret("PwVuyjLeUdVZbi4ER6yRAo0byF55AIureauV6UhLRw");
	
	string tokens_file = ofToDataPath("twitter_roxlutest.txt",true);
	if(!twitter.loadTokens(tokens_file)) {
		string auth_url;
		twitter.requestToken(auth_url);
		twitter.handlePin(auth_url);
		twitter.accessToken();
		twitter.saveTokens(tokens_file);
	}
	
	//stream.track("love");
	//stream.connect(URL_STREAM_USER);
	
	twitter.addEventListener(listener);
	*/
}

void testApp::update(){
	//stream.update();
}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	/*
	if(key == ' ') {
		twitter.statusesUpdateWithMedia("test upload", ofToDataPath("wayne.jpg",true));
		printf("Uploaded:\n");
		printf("%s\n", twitter.getResponse().c_str());
	}
	else if(key == '1') {
		twitter.statusesUpdate("test update");
	}
	*/
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