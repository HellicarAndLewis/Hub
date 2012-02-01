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

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	
	// touch stuff
	touchReceiver.setup(1234);
	touchReceiver.setListener(this);
	simulator.setEnabled(false);
	simulator.setListener(this);
	
	screenSettingsFile = "DisplayLayout.xml";
	
	renderer.setup(1920, 1080);
	renderer.setupGui();
	
	gui.addPage("Screen Settings");
	gui.addToggle("Generate Screen Layout", generateScreens);
	gui.addToggle("Load Screens File", shouldLoadScreens);
	gui.addToggle("Save Screens File", shouldSaveScreens);

	gui.loadFromXML();
	gui.setAutoSave(true);

	screenManager.loadScreens(screenSettingsFile);
	
}

void testApp::exit() {
	
}

//--------------------------------------------------------------
void testApp::update(){
	
	touchReceiver.update();
	
	renderer.update();
	renderer.render();
	
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
	
	//draw preview rects
	ofPushStyle();
	ofSetColor(255, 255, 255);
	ofNoFill();
	ofDrawBitmapString("Render Texture", screenManager.sourceRect.x+5, screenManager.sourceRect.y-5);
	ofRect(screenManager.sourceRect);

	ofSetColor(255, 255, 0);
	ofDrawBitmapString("Display Output", screenManager.destRect.x+5, screenManager.destRect.y-5);
	ofRect(screenManager.destRect);
	ofPopStyle();
	
	
	renderer.getFbo().getTextureReference().bind();
	screenManager.renderPreview();
	renderer.getFbo().getTextureReference().unbind();
	
	renderer.getFbo().getTextureReference().draw(screenManager.sourceRect);
	screenManager.renderScreens();

	
	/*
	ofSetHexColor(0x0000FF);
	for(map<int,KinectTouch>::iterator it = blobs.begin(); it != blobs.end(); it++) {
		ofCircle((*it).second.x*ofGetWidth(), (*it).second.y*ofGetHeight(), 10, 10);
	}*/
	gui.draw();
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
		case 'm': {
			simulator.setEnabled(!simulator.getEnabled());
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
	simulator.mouseDragged(x, y);
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
	simulator.mousePressed(x, y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	simulator.mouseReleased(x, y);
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
	renderer.touchDown(touch);
	blobs[touch.id] = touch;
}
void testApp::touchMoved(const KinectTouch &touch) {
	renderer.touchMoved(touch);
	blobs[touch.id] = touch;
}
void testApp::touchUp(const KinectTouch &touch) {
	renderer.touchUp(touch);
	if(blobs.find(touch.id)!=blobs.end()) {
		blobs.erase(touch.id);
	}
}