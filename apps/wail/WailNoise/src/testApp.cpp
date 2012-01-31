#include "testApp.h"
#include "ofxXmlSettings.h"
#include "AudioSystem.h"

//
//
// NOTE FROM MAREK - 
// 
// I made a new project called "WailWall" which is for the visualization.
//
// This project is just for the kinect reading, and will have OSC going out
// to the "WailWall" app.
//


//--------------------------------------------------------------
void testApp::setup(){
	
	kinect.setup(2468);
	kinect.setListener(this);
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	
	slosh.setup("bubbles/bubble", 11);
	slosh.setProbability(0.3);
	slosh.setVolumeRange(0.1, 0.5);
	drop.setup("drop/drop", 11);
	drop.setProbability(1);
	drop.setVolumeRange(0.8, 1);
	//gui.setup();
	//gui.addDrawable("Depth", depthImg);
	
	gui.setEnabled(true);
 
	gui.loadSettings("wailnoise.xml");
	ofSetWindowTitle("WailNoise");
	audio::init(2);
	audio::SampleRef bgLoop = audio::loadSample(ofToDataPath("bgloop.wav"));
	bgLoopPlayer = audio::createPlayer(bgLoop);
	audio::setLooping(bgLoopPlayer, true);
	audio::setVolume(bgLoopPlayer, 0.05);
	audio::play(bgLoopPlayer);
	ofSoundStreamSetup(2, 0, 44100, 512, 1);

	//ofSoundStreamStart();
	
}

void testApp::audioOut(float * output, int bufferSize, int nChannels) {
	audio::getAudioSystem()->getSamples(output, bufferSize, nChannels);
}

void testApp::exit() {
}



//--------------------------------------------------------------
void testApp::update(){
	kinect.update();
	
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	ofSetHexColor(0xFFFFFF);
	ofDrawBitmapString("WAILNOISE", 10, 20);

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

void testApp::touchDown(const KinectTouch &touch) {
	drop.trigger(ofVec3f(ofClamp(touch.x, 0, 1), 0, 0));
}

void testApp::touchMoved(const KinectTouch &touch) {
	slosh.trigger(ofVec3f(ofClamp(touch.x, 0, 1), 0, 0));
}

void testApp::touchUp(const KinectTouch &touch) {
	
}