#include "testApp.h"
#include "ofxXmlSettings.h"

//
//
// NOTE FROM MAREK - 
// 
// I made a new project called "WailWall" which is for the visualization.
//
// This project is just for the kinect reading, and will have OSC going out
// to the "WailWall" app.
//

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
	
	setupMask();
	
	
	//gui.setup();
	//gui.addDrawable("Depth", depthImg);
	gui.addSlider("Water Surface Level", waterThreshold, 0, 255);
	gui.setEnabled(true);
 
	gui.loadSettings("wailwell.xml");
	
	gui.position(10, 510);
}
void testApp::setupMask() {
	// defaults for mask
	mask[0] = ofVec2f(320, 50);
	mask[1] = ofVec2f(590, 240);
	mask[2] = ofVec2f(320, 430);
	mask[3] = ofVec2f(50, 240);
	dragger = NULL;
	
	ofxXmlSettings xml;
	xml.loadFile("mask.xml");
	xml.pushTag("mask");
	int numPoints = MIN(NUM_MASK_POINTS, xml.getNumTags("point"));
	for(int i = 0; i < numPoints; i++) {
		mask[i] = ofVec2f(xml.getAttribute("point", "x", 0.0, i), xml.getAttribute("point", "y", 0.0, i));
	}
	
}

void testApp::saveMask() {
	ofxXmlSettings xml;
	xml.addTag("mask");
	xml.pushTag("mask");
	for(int i = 0; i < NUM_MASK_POINTS; i++) {
		xml.addTag("point");
		xml.addAttribute("point", "x", mask[i].x, i);
		xml.addAttribute("point", "y", mask[i].y, i);
	}
	xml.saveFile("mask.xml");
	
}
void testApp::exit() {
	kinect.close();
}

float lastVisionCalculationDuration = 0;

//--------------------------------------------------------------
void testApp::update(){
	kinect.update();
	if(kinect.isFrameNew()) {
		float startTime = ofGetElapsedTimef();
		depthImg.setFromPixels(kinect.getDepthPixels(),640,480);
		
		cvThreshold(depthImg.getCvImage(), depthImg.getCvImage(), waterThreshold, 255, CV_THRESH_TOZERO_INV);
		
		
		depthImg.flagImageChanged();
		
		CvPoint points[NUM_MASK_POINTS];
		for(int i = 0; i < NUM_MASK_POINTS; i++) {
			points[i] = cvPoint(mask[i].x, mask[i].y);
		}
		
		
		CvPoint fill[4];
		fill[0] = cvPoint(0, 0);
		fill[1] = cvPoint(640, 0);
		fill[2] = cvPoint(640, 480);
		fill[3] = cvPoint(0, 480);
		
		CvPoint *allPoints[2];
		allPoints[0] = points;
		allPoints[1] = fill;
		
		int numPoints[2];
		numPoints[0] = NUM_MASK_POINTS;
		numPoints[1] = 4;
		
		// mask out the bit we're interested in
		cvFillPoly(
				   depthImg.getCvImage(), allPoints,
				   numPoints,
				   2, cvScalar(0)
				   );
		depthImg.flagImageChanged();
		
		lastVisionCalculationDuration = ofGetElapsedTimef() - startTime;
	}
	
	
	ofSetWindowTitle("WailWell - "+ ofToString(ofGetFrameRate(), 2) + "fps - " + ofToString(lastVisionCalculationDuration*1000, 0) + "ms");
	
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	
	ofSetHexColor(0xFFFFFF);
	kinect.drawDepth(0, 0, ofGetWidth(), ofGetHeight());
	depthImg.draw(10, 10, 640, 480);
	ofNoFill();
	ofRect(10, 10, 640, 480);
	ofFill();
	
	
	glPushMatrix();
	glTranslatef(10, 10, 0);
	for(int i = 0; i < NUM_MASK_POINTS; i++) {
		if(dragger==&mask[i]) {
			ofSetHexColor(0x00FF00);
		} else {
			ofSetHexColor(0xFF0000);
		}
		ofRect(mask[i].x-1, mask[i].y-1, 2, 2);
	}
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
	ofVec2f mouse(x-10, y-10);
	for(int i = 0; i < NUM_MASK_POINTS; i++) {
		if(mask[i].distanceSquared(mouse)<100) {
			dragger = &mask[i];
			return;
		}
	}
	dragger = NULL;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	ofVec2f mouse(x-10, y-10);
	if(dragger!=NULL) {
		dragger->x = mouse.x;
		dragger->y = mouse.y;
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	ofVec2f mouse(x-10, y-10);
	if(dragger!=NULL) {
		dragger->x = mouse.x;
		dragger->y = mouse.y;
	}
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	saveMask();
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