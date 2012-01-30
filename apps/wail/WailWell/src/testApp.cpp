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


//--------------------------------------------------------------
void testApp::setup(){
	waterThreshold = 10;
	maxWaterDepth = 255;
	minBlobSize = 10;
	maxBlobSize = 200;
	kinect.init();
	kinect.open();
	kinect.setUseTexture(false);
	depthImg.allocate(kinect.getWidth(), kinect.getHeight());
	
	blobTracker.setListener(this);
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	
	setupMask();
	
	
	//gui.setup();
	//gui.addDrawable("Depth", depthImg);
	gui.addSlider("Water Surface Level", waterThreshold, 0, 255);
	gui.addSlider("Maximum Water Depth", maxWaterDepth, 0, 255);
	gui.addSlider("Minimum Blob Size", minBlobSize, 10, 320);
	gui.addSlider("Maximum Blob Size", maxBlobSize, 320, 640);
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
		
		
		// threshold anything too close to the camera
		cvThreshold(depthImg.getCvImage(), depthImg.getCvImage(), maxWaterDepth, 255, CV_THRESH_TOZERO_INV);
		depthImg.flagImageChanged();
		
		// threshold anything too far away from the camera - i.e. the surface of the water
		cvThreshold(depthImg.getCvImage(), depthImg.getCvImage(), waterThreshold, 255, CV_THRESH_TOZERO);
		depthImg.flagImageChanged();
		
		
		
		//--------------------------------------------------------------------------------
			// map from min1-max1 to min2-max2
			float scale = 255/(maxWaterDepth-waterThreshold);
			cvConvertScale( depthImg.getCvImage(), depthImg.getCvImage(), scale, -(waterThreshold*scale));
		
		
		
//		depthImg.rangeMap(waterThreshold, maxWaterDepth, 0, 255);
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
		
		contourFinder.findContours(depthImg, minBlobSize*minBlobSize, maxBlobSize*maxBlobSize, 20, false);
		blobTracker.trackBlobs(contourFinder.blobs);
		lastVisionCalculationDuration = ofGetElapsedTimef() - startTime;
	}
	
	
	ofSetWindowTitle("WailWell - "+ ofToString(ofGetFrameRate(), 2) + "fps - " + ofToString(lastVisionCalculationDuration*1000, 0) + "ms");
	
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	
	glPushMatrix();
	{
		glTranslatef(10, 10, 0);
	
		ofSetHexColor(0xFFFFFF);
		kinect.drawDepth(0, 0, ofGetWidth(), ofGetHeight());
		depthImg.draw(0, 0, 640, 480);
		//contourFinder.draw(0, 0);
		ofNoFill();
		ofRect(10, 10, 640, 480);
		ofFill();
		blobTracker.draw(0, 0);
		
		
		
		for(int i = 0; i < NUM_MASK_POINTS; i++) {
			if(dragger==&mask[i]) {
				ofSetHexColor(0x00FF00);
			} else {
				ofSetHexColor(0xFF0000);
			}
			ofRect(mask[i].x-1, mask[i].y-1, 2, 2);
		}
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

// callbacks for blob listener
void testApp::blobOn( int x, int y, int id, int order ) {
	
}
void testApp::blobMoved( int x, int y, int id, int order ) {
	
}
void testApp::blobOff( int x, int y, int id, int order ) {
	
}