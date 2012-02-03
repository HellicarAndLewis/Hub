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

	glEnable(GL_DEPTH_TEST);
	ofEnableAlphaBlending();
	//ofSoundStreamStart();
	//ofDisableSetupScreen();
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
	glPushMatrix();
	glTranslatef(ofGetWidth()/2, ofGetHeight()/2, 0);
	glScalef(ofGetHeight()/2, ofGetHeight()/2, 1);
	//glTranslatef(0.5, 0.5, 0.5);
	glRotatef(mouseX, 0, 1, 0);
	glRotatef(mouseY, 1, 0, 0);
	glTranslatef(-0.5, -0.5, -0.5);	
	ofEnableAlphaBlending();
	glColor4f(0, 0.1, 0.2 , 0.5);
	glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	glVertex3f(1, 1, 0);
	glVertex3f(0, 1, 0);
	glEnd();
	
	glColor4f(1, 1, 1, 1);
	
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(1, 0, 0);
	
	glVertex3f(0, 0, 0);
	glVertex3f(0, 1, 0);
	
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);
	
	glVertex3f(1, 1, 0);
	glVertex3f(0, 1, 0);
	
	glVertex3f(1, 1, 0);
	glVertex3f(1, 0, 0);
	
	glVertex3f(1, 1, 0);
	glVertex3f(1, 1, 1);
	
	glVertex3f(0, 1, 1);
	glVertex3f(1, 1, 1);
	
	glVertex3f(0, 1, 1);
	glVertex3f(0, 0, 1);
	
	glVertex3f(0, 1, 1);
	glVertex3f(0, 1, 0);
	
	glVertex3f(1, 0, 1);
	glVertex3f(1, 1, 1);
	
	glVertex3f(1, 0, 1);
	glVertex3f(0, 0, 1);
	
	glVertex3f(1, 0, 1);
	glVertex3f(1, 0, 0);
	
	
	glEnd();

//	glBegin(GL_POINTS);
//	glPointSize(3);
	glColor3f(1, 0, 0);
	for(map<int,ofVec3f>::iterator it = blobs.begin(); it != blobs.end(); it++) {
		ofSphere((*it).second, 0.03);
	}
//	glEnd();
	glPopMatrix();
	

	

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
	blobs[touch.id] = ofVec3f(touch.x, touch.y, touch.z);
	drop.trigger(ofVec3f(ofClamp(touch.x, 0, 1), 0, 0));
}

void testApp::touchMoved(const KinectTouch &touch) {
	blobs[touch.id] = ofVec3f(touch.x, touch.y, touch.z);
	slosh.trigger(ofVec3f(ofClamp(touch.x, 0, 1), 0, 0));
}

void testApp::touchUp(const KinectTouch &touch) {
	if(blobs.find(touch.id)!=blobs.end()) {
		blobs.erase(touch.id);
	}
}