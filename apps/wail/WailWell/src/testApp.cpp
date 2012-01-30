#include "testApp.h"
#include "ofxSimpleGuiToo.h"

///*******
//JG Layout calculations
//Physical width across 
//		22.83 x 5 = 114.15 inches (with bezel) 
//		22.54 x 5 = 112.70 inches (without bezel)
//Physical height
//		40.38 x 3 = 121.14 inches (with bezel)
//		40.09 x 3 = 120.27 inches (without bezel)
//That means our aspect ratio of the whole screen is
//	114.15 / 120.27 = .95

//and our aspect ratio per row is
//	114.15 / 40.38 = 2.83
//*******

int canvasWidth  = 720*5;
int canvasHeight = 720*5;

float waterThreshold = 10;	
//--------------------------------------------------------------
void testApp::setup(){
	kinect.init();
	kinect.open();
	kinect.setUseTexture(false);
	depthImg.allocate(kinect.getWidth(), kinect.getHeight());
	
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	
	screenSettingsFile = "ScreenSettings.xml";
	
	gui.addSlider("Water Surface Level", waterThreshold, 0, 255);
	
	gui.addPage("Screen Settings");
	gui.addToggle("Generate Screen Layout", generateScreens);
	gui.addToggle("Load Screens File", shouldLoadScreens);
	gui.addToggle("Save Screens File", shouldSaveScreens);
	
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
	
	if(generateScreens){
		generateScreens = false;
		screenManager.generateScreens(5, 3);
		screenManager.saveScreens(screenSettingsFile);
	}
	
	if(shouldLoadScreens){
		shouldLoadScreens = false;
		screenManager.loadScreens(screenSettingsFile);
	}
	
	if(shouldSaveScreens){
		shouldSaveScreens = false;
		screenManager.saveScreens(screenSettingsFile);
	}	
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	
//	ofSetHexColor(0xFFFFFF);
//	kinect.drawDepth(0, 0, ofGetWidth(), ofGetHeight());
	
	//draw preview rects
	ofPushStyle();
	ofSetColor(255, 255, 255);
	ofNoFill();
	ofDrawBitmapString("Render Texture", screenManager.sourceRect.x+5, screenManager.sourceRect.y-5);
	ofRect(screenManager.sourceRect);

	ofSetColor(255, 255, 0);
	ofDrawBitmapString("Display Output", screenManager.destRect.x+5, screenManager.destRect.y-5);
	ofRect(screenManager.destRect);
	
	screenManager.renderPreview();
	screenManager.renderScreens();
	
	ofPopStyle();
	
	if(gui.isOn()) gui.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch(key) {
		case ' ': {
			gui.toggleDraw();
			break;
		
		}
		case 'f': {
			ofToggleFullscreen();
			break;
		}
			
		case 'l': {
			shouldLoadScreens = true;
			break;
		}
			
		case 's': {
			shouldSaveScreens = true;
			break;
		}
			
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
	if(ofGetKeyPressed('p')){
		screenManager.sourceRect.width =  x - screenManager.sourceRect.x;
		screenManager.sourceRect.height = y - screenManager.sourceRect.y;
	}	
	
	if(ofGetKeyPressed('d')){
		screenManager.destRect.width =  x - screenManager.destRect.x;
		screenManager.destRect.height = y - screenManager.destRect.y;
	
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	if(ofGetKeyPressed('p')){
		screenManager.sourceRect.x = x;
		screenManager.sourceRect.y = y;
	}
	else if(ofGetKeyPressed('d')){
		screenManager.destRect.x = x;
		screenManager.destRect.y = y;
	}
	else {
		//test selections
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