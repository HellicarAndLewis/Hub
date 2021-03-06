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


float damp = 0;
float roomSize = 0;
float wet = 0;
float dry = 0;
float width = 0;

float pan = 0.5;
float panTarget = 0.5;
// these are the VU's
float depth = 0;
float disturbance = 0;

float targetDepth = 0;
float targetDisturbance = 0;

float depthUpLerp = 0.92;
float depthDownLerp = 0.92;

float maxLowVolume = 0.2;
float maxHighVolume = 0.2;

float disturbanceUpLerp = 0.92;
float disturbanceDownLerp = 0.92;

float disturbanceGain = 10;

// these are constants
float PREVIEW_WIDTH = 500;
float PREVIEW_HEIGHT = 500;
float minSplashDepth = 0;
float maxSplashDepth = 0;

float bgCutoff = 8000;
float bgRes = 0.1;


#define ROTATE 0
#define SIMULATE 1
int mouseMode = ROTATE;

void drawCube();

//--------------------------------------------------------------
void testApp::setup(){
	
	kinect.setup(2468);
	kinect.setListener(this);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	

	gui.setEnabled(true);
    gui.addListener(this);
	
	bgLoopVolume = 0.05;
    bubbleVolume = 0.5;
	splashVolume = 1;
	gui.addSlider("Splash Volume", splashVolume, 0, 1);
	gui.addSlider("Bubble Volume", bubbleVolume, 0, 1);

	gui.addSlider("Background Loop Volume", bgLoopVolume, 0, 2.5);
	gui.addSlider("Background Cutoff", bgCutoff, 50, 1000);
	gui.addSlider("Background Resonance", bgRes, 0, 0.9);
	
	gui.addSlider("Min splash depth", minSplashDepth, 0, 1);
	gui.addSlider("Max splash depth", maxSplashDepth, 0, 1);

	gui.addColumn();
	gui.addSegmented("Mouse Mode", mouseMode, "ROTATE|SIMULATE");
	gui.addSlider("Max Low Volume", maxLowVolume, 0, 1);
	gui.addSlider("Max High Volume", maxHighVolume, 0, 1);
	
	gui.addSlider("DAMP", damp, 0, 1);
	gui.addSlider("ROOMSIZE", roomSize, 0, 1);
	gui.addSlider("WET", wet, 0, 1);
	gui.addSlider("DRY", dry, 0, 1);
	gui.addSlider("WIDTH", width, 0, 1);
	
	
	gui.addColumn();
	gui.addMeter("Depth", depth);
	gui.addSlider("Depth Up Lerp", depthUpLerp, 0.8, 0.96);
	gui.addSlider("Depth Down Lerp", depthDownLerp, 0.8, 0.99);
	gui.addMeter("Disturbance", disturbance);
	gui.addSlider("Disturbance Gain", disturbanceGain, 1, 20);
	gui.addSlider("Disturbance Up Lerp", disturbanceUpLerp, 0.8, 0.999);
	gui.addSlider("Disturbance Down Lerp", disturbanceDownLerp, 0.9, 0.999);
	gui.loadSettings("wailnoise.xml");
	gui.y = PREVIEW_HEIGHT + 10;
	oscInterface.setup(gui, 1098, 1097);
	
	ofSetWindowTitle("WailNoise");
	
	
	audio::init(2);

	
	reverbBus = audio::createBus(0);
	reverb = audio::createEffect(reverbBus, audio::EFFECT_TYPE_REVERB);
	
	
	for(int i = 0; i < 3; i++) {
		lows[i] = audio::createPlayer(audio::loadSample(ofToDataPath("low/low"+ofToString(i)+".wav")));
		audio::setLooping(lows[i], true);
		audio::playOnBus(lows[i], reverbBus);
		audio::setVolume(lows[i], maxLowVolume);
	}
	
	for(int i = 0; i < 1; i++) {
		highs[i] = audio::createPlayer(audio::loadSample(ofToDataPath("high/high"+ofToString(i)+".wav")));
		audio::setLooping(highs[i], true);
		audio::playOnBus(highs[i], reverbBus);
		audio::setVolume(highs[i], maxHighVolume);
	}


	

	
	
	audio::BusRef hiPassBus = audio::createBus(0);
	audio::setBusVolume(hiPassBus, 0.9);
	
	hiPass = audio::createEffect(hiPassBus, audio::EFFECT_TYPE_HI_PASS);
	audio::setEffectParameter(hiPass, FilterEffect::CUTOFF, bgCutoff);
	audio::setEffectParameter(hiPass, FilterEffect::RESONANCE, bgRes);	
	

	
	// background loop
	audio::SampleRef bgLoop = audio::loadSample(ofToDataPath("bgloop.wav"));
	bgLoopPlayer = audio::createPlayer(bgLoop);
	audio::setLooping(bgLoopPlayer, true);
	audio::setVolume(bgLoopPlayer, bgLoopVolume);
	audio::playOnBus(bgLoopPlayer, reverbBus);
	
	//audio::playOnBus(bgLoopPlayer, hiPassBus);
	
	// initial splash
	splash.setup("plop");
	splash.setProbability(1);
	splash.setVolumeRange(0.9, 1);
	
	
	
    //slosh.setup("splish");
	//slosh.setProbability(0.3);
	//slosh.setVolumeRange(0.2*bubbleVolume, bubbleVolume);
    

    
	ofSoundStreamSetup(2, 0, 44100, 512, 1);

	
	ofEnableAlphaBlending();
	//ofSoundStreamStart();
	//ofDisableSetupScreen();
}

void testApp::controlChanged(xmlgui::Control *ctrl) {
	if(ctrl->id=="Background Loop Volume") {
		audio::setVolume(bgLoopPlayer, bgLoopVolume);
	} else if(ctrl->id=="Bubble Volume") {
		slosh.setVolumeRange(0.2*bubbleVolume, bubbleVolume);
	} else if(ctrl->id=="Splash Volume") {
		splash.setVolumeRange(splashVolume*0.8, splashVolume);
	} else if(ctrl->value==&bgCutoff) {
		audio::setEffectParameter(hiPass, FilterEffect::CUTOFF, bgCutoff);
	} else if(ctrl->value==&bgRes) {
		audio::setEffectParameter(hiPass, FilterEffect::RESONANCE, bgRes);
	} else if(ctrl->value==&roomSize) {
		audio::setEffectParameter(reverb, ReverbEffect::ROOMSIZE, roomSize);
	} else if(ctrl->value==&damp) {
		audio::setEffectParameter(reverb, ReverbEffect::DAMP, damp);
	} else if(ctrl->value==&wet) {
		audio::setEffectParameter(reverb, ReverbEffect::WET, wet);
	} else if(ctrl->value==&dry) {
		audio::setEffectParameter(reverb, ReverbEffect::DRY, dry);
	} else if(ctrl->value==&width) {
		audio::setEffectParameter(reverb, ReverbEffect::WIDTH, width);
	}
	
	
}

void testApp::audioOut(float * output, int bufferSize, int nChannels) {
	audio::getAudioSystem()->getSamples(output, bufferSize, nChannels);
}

void testApp::exit() {
}

void testApp::setLowVolume(float vol) {
	for(int i = 0; i < 3; i++) {
		audio::setVolume(lows[i], maxLowVolume*vol);
	}
}

void testApp::setHighVolume(float vol) {
	for(int i = 0; i < 1; i++) {
		audio::setVolume(highs[i], maxHighVolume*vol);
	}
}
void testApp::setPan(float pan) {
	float p = ofMap(pan, 0, 1, 0.25, 0.75);
	for(int i = 0; i < 1; i++) {
		audio::setPan(highs[i], p);
	}
	for(int i = 0; i < 3; i++) {
		audio::setPan(lows[i], p);
	}
}

//--------------------------------------------------------------
void testApp::update(){
	kinect.update();
	oscInterface.update();
	if(targetDepth>depth) {
		depth = depth * depthUpLerp + targetDepth * (1.f - depthUpLerp);
	} else {
		depth = depth * depthDownLerp + targetDepth * (1.f - depthDownLerp);
	}
	
	if(targetDisturbance>disturbance) {
		disturbance = disturbance * disturbanceUpLerp + targetDisturbance * (1.f - disturbanceUpLerp);
	} else {
		disturbance = disturbance * disturbanceDownLerp + targetDisturbance * (1.f - disturbanceDownLerp);
	}
	
	depth = ofClamp(depth, 0, 1);
	disturbance = ofClamp(disturbance, 0, 1);
	setLowVolume(depth * disturbance);
	setHighVolume((1.f-depth)*disturbance);
	pan = panTarget * 0.1 + pan * 0.9;
	setPan(pan);
	
}
string messageString = "";

//--------------------------------------------------------------
void testApp::draw(){
	glEnable(GL_DEPTH_TEST);
	ofBackground(0);
	ofSetHexColor(0xFFFFFF);
	ofDrawBitmapString(messageString, 20, 20);

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
KinectTouch fakeTouch;

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	if(x<PREVIEW_WIDTH &&y<PREVIEW_HEIGHT) {
		if(mouseMode==ROTATE) {
			if(dragging) {
				ofVec3f delta = ofVec3f(x,y) - mouseStart;
				rotation = oldRotation + delta;
			}
		} else {
			
			// SIMULATE
			float xx = ofMap(x, 0, PREVIEW_WIDTH, 0, 1, true);
			float yy = ofMap(y, 0, PREVIEW_HEIGHT, 0, 1, true);
			
			fakeTouch.vel.x = xx - fakeTouch.x;
			fakeTouch.vel.y = yy - fakeTouch.y;
			
			
			fakeTouch.x = xx;
			fakeTouch.y = yy;
			fakeTouch.z = fakeTouch.z*0.9 + 0.1*yy;
			
			fakeTouch.age++;
			touchMoved(fakeTouch);
		}
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
	if(x<PREVIEW_WIDTH && y<PREVIEW_HEIGHT) {
		if(mouseMode==ROTATE) {
			dragging = true;
			mouseStart = ofVec3f(x, y);
			oldRotation = rotation;
		} else {
			// SIMULATE
			fakeTouch.x = ofMap(x, 0, PREVIEW_WIDTH, 0, 1, true);
			fakeTouch.y = ofMap(y, 0, PREVIEW_HEIGHT, 0, 1, true);
			fakeTouch.z = ofRandom(0.1, 0.2);
			fakeTouch.age = 0;
			touchDown(fakeTouch);
		}
	}

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	if(x<PREVIEW_WIDTH && y<PREVIEW_HEIGHT) {
		if(mouseMode==SIMULATE) {
			// SIMULATE
			float xx = ofMap(x, 0, PREVIEW_WIDTH, 0, 1, true);
			float yy = ofMap(y, 0, PREVIEW_HEIGHT, 0, 1, true);
			
			fakeTouch.vel.x = xx - fakeTouch.x;
			fakeTouch.vel.y = yy - fakeTouch.y;
			fakeTouch.age++;
			fakeTouch.x = xx;
			fakeTouch.y = yy;
			touchUp(fakeTouch);
		}
	}
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
	printf("Touch down\n");
	
	
}
void testApp::chaseDepth(float z) {
	targetDepth = z;
}

void testApp::chaseDisturbance(float dxy) {
	targetDisturbance = dxy*disturbanceGain;
}
void testApp::chasePan(float pan) {
	panTarget = pan;
}
void testApp::touchMoved(const KinectTouch &touch) {
	blobs[touch.id] = touch;
	chaseDepth(touch.z);
	setPan(touch.x);
	float dxy = ofVec2f(touch.vel.x, touch.vel.y).length();
	chaseDisturbance(dxy);
	
//	slosh.trigger(ofVec3f(ofClamp(touch.x, 0, 1), 0, ofRandom(0, 1)));
	if(touch.age==1) {
		splash.trigger(
					   ofVec3f(ofClamp(touch.x, 0, 1), 0, ofMap(touch.z, minSplashDepth, maxSplashDepth, 0, 1, true))
		);
		messageString = "touch:    size:"+ofToString(touch.size, 3) + "    z:" + ofToString(touch.z, 3);
	}
	printf("Touch moved\n");
}

void testApp::touchUp(const KinectTouch &touch) {
	
	chaseDepth(0);
	chaseDisturbance(0);
	if(blobs.find(touch.id)!=blobs.end()) {
		blobs.erase(touch.id);
	}
}