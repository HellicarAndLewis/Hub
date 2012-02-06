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

float PREVIEW_WIDTH = 500;
float PREVIEW_HEIGHT = 500;
float minSplashDepth = 0;
float maxSplashDepth = 0;
void drawCube();

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
 
	gui.addSlider("Min splash depth", minSplashDepth, 0, 1);
	gui.addSlider("Max splash depth", maxSplashDepth, 0, 1);
	
	gui.loadSettings("wailnoise.xml");
	gui.y = PREVIEW_HEIGHT + 10;
	ofSetWindowTitle("WailNoise");
	audio::init(2);
	audio::SampleRef bgLoop = audio::loadSample(ofToDataPath("bgloop.wav"));
	bgLoopPlayer = audio::createPlayer(bgLoop);
	audio::setLooping(bgLoopPlayer, true);
	audio::setVolume(bgLoopPlayer, 0.05);
	audio::play(bgLoopPlayer);
	
	ofSoundStreamSetup(2, 0, 44100, 512, 1);

	
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
string messageString = "";

//--------------------------------------------------------------
void testApp::draw(){
	glEnable(GL_DEPTH_TEST);
	ofBackground(0);
	ofSetHexColor(0xFFFFFF);
	/*ofDrawBitmapString(messageString, 20, 20);

	glPushMatrix();
	{
		
		glTranslatef(PREVIEW_WIDTH/2, PREVIEW_HEIGHT/2, 0);
		glScalef(PREVIEW_WIDTH/2, PREVIEW_HEIGHT/2, 1);
		//glTranslatef(0.5, 0.5, 0.5);
		glRotatef(rotation.x, 0, 1, 0);
		glRotatef(rotation.y, 1, 0, 0);
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
		
		drawCube();

	//	glBegin(GL_POINTS);
	//	glPointSize(3);
		glColor3f(1, 0, 0);
		for(map<int,KinectTouch>::iterator it = blobs.begin(); it != blobs.end(); it++) {
			ofSphere((*it).second, 0.07*(*it).second.size);
		}
	}
	
	glPopMatrix();
	
	ofFill();
	*/
	
	glDisable(GL_DEPTH_TEST);
}

void drawCube() {
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
}
//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}


ofVec3f mouseStart;
ofVec3f oldRotation;
bool dragging;
//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	if(dragging && x<PREVIEW_WIDTH &&y<PREVIEW_HEIGHT) {
		ofVec3f delta = ofVec3f(x,y) - mouseStart;
		rotation = oldRotation + delta;
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	if(x<PREVIEW_WIDTH && y<PREVIEW_HEIGHT) {
		dragging = true;
		mouseStart = ofVec3f(x, y);
		oldRotation = rotation;
	}

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
	blobs[touch.id] = touch;
	drop.trigger(ofVec3f(ofClamp(touch.x, 0, 1), 0, 0));
	
}

void testApp::touchMoved(const KinectTouch &touch) {
	blobs[touch.id] = touch;
	slosh.trigger(ofVec3f(ofClamp(touch.x, 0, 1), 0, 0));
	if(touch.age==1) {
		messageString = "touch:    size:"+ofToString(touch.size, 3) + "    z:" + ofToString(touch.z, 3);
	}
}

void testApp::touchUp(const KinectTouch &touch) {
	if(blobs.find(touch.id)!=blobs.end()) {
		blobs.erase(touch.id);
	}
}