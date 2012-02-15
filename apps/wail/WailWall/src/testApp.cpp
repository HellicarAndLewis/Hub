#include "testApp.h"
#include "ofxWebSimpleGuiToo.h"
//#include "pcrecpp.h"

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
	
	generateScreens = false;
	shouldLoadScreens = false;
	shouldSaveScreens = false;
	shouldTakeScreenshot = false;
	previewScreenLayout = false;

	
	// touch stuff
	touchReceiver.setup(1234);
	touchReceiver.setListener(this);
	simulator.setEnabled(false);
	simulator.setListener(this);
	
	//JOEL: change this to the triplehead layout for your test
	//screenSettingsFile = "DisplayLayout_triplehead.xml";
	//DEV is for testing on smaller screens
	screenSettingsFile = "DisplayLayout_dev.xml";
	//screenSettingsFile = "DisplayLayout_bigscreen.xml";
	screenManager.loadScreens(screenSettingsFile);

	webGui.addToggle("Show Preview Rects", previewScreenLayout);
	renderer.blobs = &blobs;
	renderer.setup(screenManager.sourceRect.width, screenManager.sourceRect.height);	
	renderer.setupGui();

	//JG 2/6/12
	//Disabled for now since we are hardcoding the screen values
//	gui.addPage("Screen Settings");
//	gui.addToggle("Generate Screen Layout", generateScreens);
//	gui.addToggle("Load Screens File", shouldLoadScreens);
//	gui.addToggle("Save Screens File", shouldSaveScreens);
	
	// disable for now
	//webGui.startServer();
	webGui.loadFromXML();
	webGui.setAutoSave(true);
	

}

void testApp::exit() {
	//webGui.stopServer();
}

//--------------------------------------------------------------
void testApp::update(){
	touchReceiver.update();
	
	renderer.update();
	renderer.render();
	
	if(generateScreens){
		//danger zone for hand written files
//		generateScreens = false;
//		screenManager.generateScreens(5, 3);
//		screenManager.saveScreens(screenSettingsFile);
	}
	
	if(shouldLoadScreens){
		shouldLoadScreens = false;
		screenManager.loadScreens(screenSettingsFile);
	}
	
	if(shouldSaveScreens){
		//danger zone for hand written files
//		shouldSaveScreens = false;
//		screenManager.saveScreens(screenSettingsFile);
	}	
}

//--------------------------------------------------------------
void testApp::draw(){
	// roxlu 02/07
	ofSetFullscreen(false); 
	
	ofBackground(0);
	ofRectangle renderPreview = screenManager.getRenderPreviewRect();
	renderer.getFbo().getTextureReference().draw(renderPreview);
	if(previewScreenLayout){
//		screenManager.renderLayout();
	}
	
	renderer.getFbo().getTextureReference().bind();
	screenManager.renderScreens();
	renderer.getFbo().getTextureReference().unbind();

	if(previewScreenLayout){
		//draw preview rects
		ofPushStyle();
		ofSetColor(255, 255, 255);
		ofNoFill();
		ofDrawBitmapString("Preview", screenManager.previewRect.x+5, screenManager.previewRect.y-5);
		ofRect(screenManager.previewRect);
		ofSetColor(0, 255, 0);
		ofRect(renderPreview);
		
		ofSetColor(255, 255, 0);
		ofDrawBitmapString("Display Output", screenManager.destRect.x+5, screenManager.destRect.y-5);
		ofRect(screenManager.destRect);
		ofPopStyle();
	}	
	
	if(shouldTakeScreenshot) {
		// %Y-%m-%d-%H-%M-%S-%i
		/*
		string dirname = "thumbs/" +ofGetTimestampString("%m-%d");
		ofDirectory dir(dirname);
		dir.create(true);
		
		string filename = ofGetTimestampString() +"_" +ofToString(ofGetFrameNum()) +".png";
		string filepath(dirname);
		filepath.append("/");
		filepath.append(filename);
		
		// pretty sure we can do this better
		ofPixels pixels;
		renderer.getFbo().readToPixels(pixels);
		ofImage img;
		img.setFromPixels(pixels);
		img.saveImage(filepath);
		
		
		renderer.getTweetManager().getTwitterApp().uploadScreenshot(ofToDataPath(filepath, true), "roxlu", "");
		shouldTakeScreenshot = false;
		*/
		// %Y-%m-%d-%H-%M-%S-%i
		string dirname = "thumbs/" +ofGetTimestampString("%m-%d");
		ofDirectory dir(dirname);
		dir.create(true);

		string filename = ofGetTimestampString() +"_" +ofToString(ofGetFrameNum()) +".png";
		string filepath(dirname);
		filepath.append("/");
		filepath.append(filename);

		// pretty sure we can do this better
		ofPixels pixels;
		renderer.getFbo().readToPixels(pixels);
		ofImage img;
		img.setFromPixels(pixels);
		img.saveImage(filepath);


		renderer.getTweetManager().getTwitterApp().uploadScreenshot(ofToDataPath(filepath, true), "roxlu", "");
		shouldTakeScreenshot = false;
	}
	
	/*
	ofSetHexColor(0x0000FF);
	for(map<int,KinectTouch>::iterator it = blobs.begin(); it != blobs.end(); it++) {
		ofCircle((*it).second.x*ofGetWidth(), (*it).second.y*ofGetHeight(), 10, 10);
	}*/
	
	webGui.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch(key) {
		case ' ': {
			webGui.toggleDraw();
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
		case 'p': {
			// roxlu 02/07, test with screenshots
			shouldTakeScreenshot = !shouldTakeScreenshot;
			break;
		}
			
		case OF_KEY_LEFT: {
			webGui.prevPage();
			break;
		}
		case OF_KEY_RIGHT: {
			webGui.nextPage();
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
	//JG: disabling for now, going hard-coded
//	if(ofGetKeyPressed('p')){
//		screenManager.sourceRect.width =  x - screenManager.sourceRect.x;
//		screenManager.sourceRect.height = y - screenManager.sourceRect.y;
//	}	
//	
//	if(ofGetKeyPressed('d')){
//		screenManager.destRect.width =  x - screenManager.destRect.x;
//		screenManager.destRect.height = y - screenManager.destRect.y;
//	
//	}
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
	// see ofxWWTweetParticleManager
	if(msg.message == "take_screenshot") {
		shouldTakeScreenshot = true;
	}
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