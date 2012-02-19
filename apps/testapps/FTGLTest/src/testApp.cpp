#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(22,33,44);
	font.loadFont("fonts/montreal-ttf/Montreal-LightIta.ttf", 130, true, true, false);
	
	string str = "RT @Natalie10Jones: When you tell me you love me ❤❤❤❤❤❤❤❤❤❤❤❤❤❤❤💙💙💙💙💙💙💙💙💜💜💜💜💜💜💜💛💛💛💛💛💗💗💗💗❤💚💚💙❤💜💚💜❤💜💙❤❤💗💜💜💚💙💚💙❤💗💜💚💛💓";
	string::iterator it = utf8::find_invalid(str.begin(), str.end());
	if(it != str.end()) {
		printf("Invali\n");
	}
	/*
	 string::iterator end_it = utf8::find_invalid(line.begin(), line.end());
        if (end_it != line.end()) {
	*/
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	ofEnableAlphaBlending();
	font.drawString("RT @Natalie10Jones: When you tell me you love me ❤❤❤❤❤❤❤❤❤❤❤❤❤❤❤💙💙💙💙💙💙💙💙💜💜💜💜💜💜💜💛💛💛💛💛💗💗💗💗❤💚💚💙❤💜💚💜❤💜💙❤❤💗💜💜💚💙💚💙❤💗💜💚💛💓", 100,100);
//	font.drawString(" being 😞 clogged SOME STRING  ", 100,300);
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