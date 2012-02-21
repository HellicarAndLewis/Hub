#include "testApp.h"
#include "ofxWebSimpleGuiToo.h"
#include "Error.h"
#include <time.h>

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
	/*
	string time_str = "Mon Feb 20 19:30:17 +0000 2012";

	struct tm tm;
	strptime(time_str.c_str(), "%a %b %d %H:%M:%S %z %Y", &tm);
	
	// Convert a tm struct into a tme string.
	char buffer[80];
	strftime(buffer, 80, "%a %b %d %H:%M:%S %z %Y", &tm);
	printf("input: %s  output: %s\n", time_str.c_str(), buffer);
	::exit(0);
	*/
	ofSetLogLevel(OF_LOG_ERROR); // roxlu 16/02 Getting: OF: OF_LOG_WARNING: ofMap: avoiding possible divide by zero, check inputMin and inputMax
 
	ofSetFrameRate(60);
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
	cout << "setting up renderer" << endl;
	renderer.setup(screenManager.sourceRect.width, screenManager.sourceRect.height);	
	renderer.setupGui();

	// disable for now
	//webGui.startServer();
	webGui.loadFromXML();
	webGui.setAutoSave(true);
	
	glGetError();

	screen_w = ofGetWidth();
	screen_h = ofGetHeight();
	screen_w = 3840;
	screen_h = 3072;
	
	//screen_w = 1024;
	//screen_h = 768;
	
	screen_w = screen_w / 4;
	screen_h = screen_h / 4;
	printf("================================= %d %d\n", renderer.getFbo().getWidth(), renderer.getFbo().getHeight());
	int size = screen_w * screen_h * 3;
	glGenBuffers(1,&pbo);  eglGetError();
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo); eglGetError();
	glBufferData(GL_PIXEL_PACK_BUFFER, size, NULL, GL_STATIC_READ); eglGetError();
	
	renderer.getSearchTermManager().setScreenshotCallback(&testApp::theScreenshotCallback, this);
	
}

void testApp::exit() {
	//webGui.stopServer();
	renderer.stopFluidThread();
}

//--------------------------------------------------------------
void testApp::update(){
	touchReceiver.update();
	
	renderer.update();
	renderer.render();
}

//--------------------------------------------------------------
void testApp::theScreenshotCallback(const string& username, void* userdata) {
	testApp* app = static_cast<testApp*>(userdata);
	app->screenshotUsername = username;
	app->shouldTakeScreenshot = true;
}

void testApp::draw(){
	// roxlu 02/07
	//ofSetFullscreen(false); 
	
	ofBackground(0);
	ofRectangle renderPreview = screenManager.getRenderPreviewRect();
	renderer.getFbo().getTextureReference().draw(renderPreview);
	
	renderer.getFbo().getTextureReference().bind();
	screenManager.renderScreens();
	renderer.getFbo().getTextureReference().unbind();
	

	renderer.getScreenshotFbo().begin();
	renderer.getFbo().draw(0,0,screen_w, screen_h);
	renderer.getScreenshotFbo().end();
	

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
		string dirname = "thumbs/" +ofGetTimestampString("%m-%d");
		ofDirectory dir(dirname);
		dir.create(true);

		string filename = ofGetTimestampString() +"_" +ofToString(ofGetFrameNum()) +".jpg";
		string filepath(dirname);	
		filepath.append("/");
		filepath.append(filename);
		
		glGetError();
		glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo); eglGetError();
		renderer.getScreenshotFbo().getTextureReference().bind(); eglGetError();
		
		glGetTexImage(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, GL_UNSIGNED_BYTE, 0); eglGetError();
		GLubyte* ptr = 	(GLubyte*) glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		
		if(ptr) {
			renderer.getTweetManager()
					.getTwitterApp()
					.getImageWriter()
					.addPixels(filepath, screenshotUsername, ptr, screen_w, screen_h);
			
					
		}
		
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		shouldTakeScreenshot = false;
		screenshotUsername.clear();
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
			shouldTakeScreenshot = true;
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