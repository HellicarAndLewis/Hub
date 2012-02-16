#include "testApp.h"
#include "ofxXmlSettings.h"



int KINECT_VIEW_OFFSET = 10;

float Blob::xySmoothing = 0.3;
float Blob::zSmoothing = 0.4;	
bool isFullscreen = false;
//--------------------------------------------------------------
void testApp::setup(){
	
	xScaleTop = 1;
	xScaleBottom = 1;
	yScaleTop = 1;
	yScaleBottom = 1;
	flipX = false;
	flipY = false;
	doInpainting = false;
	viewMode = VIEWMODE_RAW;
	wallOsc.setup("localhost", 1234);
	soundOsc.setup("localhost", 2468);
	blurSize = 0;
	blurIterations = 0;
	waterThreshold = 0.75;
	maxWaterDepth = 1;
	minBlobSize = 10;
	maxBlobSize = 200;


	accumulateBackground = false;
	backgroundAccumulationCount = 0;
	
	drawBlobs = true;
	erode = false;
	dilate = false;
	
	
	
	inpainter.setup(640, 480, 8);
	inpainter.setInpaintingRadius(10);
	setupVision();
	
	
	blobTracker.setListener(this);
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	
	setupMask();
	
	
	setupGui();
}


void testApp::setupVision() {
	
	kinect.init();
	kinect.open();
	kinect.setUseTexture(false);
	KINECT_WIDTH = kinect.getWidth();
	KINECT_HEIGHT = kinect.getHeight();
	depthImg.allocate(KINECT_WIDTH, KINECT_HEIGHT);
	rangeScaledImg.allocate(KINECT_WIDTH, KINECT_HEIGHT);
	maskedImg.allocate(KINECT_WIDTH, KINECT_HEIGHT);
	bgImg.allocate(KINECT_WIDTH, KINECT_HEIGHT);
	bgImg.set(0);

}


void testApp::setupGui() {
	gui.setWidth(206);
	gui.addSegmented("View", viewMode, "RAW|BG|RANGE|MASKED|MUTE");
	gui.addSlider("Blur Size", blurSize, 0, 10)->stepped = true;
	gui.addSlider("Blur Iterations", blurIterations, 0, 10)->stepped = true;
	gui.addToggle("Erode Image", erode);
	gui.addToggle("Dilate Image", dilate);
	gui.addToggle("Do Inpainting", doInpainting);
	gui.addPushButton("Accumulate Background")->width = 150;
	gui.addSlider("Background Hysteresis", backgroundHysteresis, 0, 0.1);
	gui.addColumn();
	gui.addToggle("Draw Blobs", drawBlobs);
	gui.addSlider("Water Surface Level", waterThreshold, 0.75, 1);//->stepped = true;
	gui.addSlider("Maximum Water Depth", maxWaterDepth, 0.75, 1);//->stepped = true;
	gui.addSlider("Minimum Blob Size", minBlobSize, 10, KINECT_WIDTH/2)->stepped = true;
	gui.addSlider("Maximum Blob Size", maxBlobSize, KINECT_WIDTH/2, KINECT_WIDTH)->stepped = true;
	gui.addSlider("Blob XY Smoothing", Blob::xySmoothing, 0, 1);
	gui.addSlider("Blob Z Smoothing", Blob::zSmoothing, 0, 1);
	gui.addColumn();
	
	gui.addSlider("Blob x top frustrum factor", xScaleTop, 1, 8);
	gui.addSlider("Blob y top frustrum factor", yScaleTop, 1, 8);
	
	gui.addSlider("Blob x bottom frustrum factor", xScaleBottom, 1, 8);
	gui.addSlider("Blob y bottom frustrum factor", yScaleBottom, 1, 8);
	gui.addToggle("Flip X", flipX);
	gui.addToggle("Flip Y", flipY);
	
	gui.setEnabled(true);
	
	gui.loadSettings("wailwell.xml");
	
	gui.position(10, 510);
	gui.addListener(this);
}



void testApp::controlChanged(xmlgui::Control *ctrl) {
	if(ctrl->id=="Accumulate Background") {
		accumulateBackground = true;
		backgroundAccumulationCount = 0;
		bgImg.set(0);

	}
}
void testApp::setupMask() {
	// defaults for mask
	mask[0] = ofVec2f(KINECT_WIDTH/2, 50);
	mask[1] = ofVec2f(KINECT_WIDTH-50, KINECT_HEIGHT/2);
	mask[2] = ofVec2f(KINECT_WIDTH/2, KINECT_HEIGHT-50);
	mask[3] = ofVec2f(50, KINECT_HEIGHT/2);
	dragger = NULL;
	
	ofxXmlSettings xml;
	xml.loadFile("mask.xml");
	xml.pushTag("mask");
	int numPoints = MIN(NUM_MASK_POINTS, xml.getNumTags("point"));
	for(int i = 0; i < numPoints; i++) {
		mask[i] = ofVec2f(xml.getAttribute("point", "x", 0.0, i), xml.getAttribute("point", "y", 0.0, i));
		if(mask[i].x<0) mask[i].x = 0;
		if(mask[i].y<0) mask[i].y = 0;
		if(mask[i].x>KINECT_WIDTH) mask[i].x = KINECT_WIDTH;
		if(mask[i].y>KINECT_HEIGHT) mask[i].y = KINECT_HEIGHT;
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




//--------------------------------------------------------------
void testApp::update(){
	kinect.update();
	if(kinect.isFrameNew()) {
		doVision();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(22, 33, 44);
	
	glPushMatrix();
	{
		if(isFullscreen) {
			glScalef((float)ofGetWidth()/(float)KINECT_WIDTH, (float)ofGetHeight()/(float)KINECT_HEIGHT, 1);
		} else {
			glTranslatef(KINECT_VIEW_OFFSET, KINECT_VIEW_OFFSET, 0);
		}
		
		ofSetHexColor(0xFFFFFF);

		drawKinect();
		
		ofNoFill();
		ofRect(0, 0, KINECT_WIDTH, KINECT_HEIGHT);
		ofFill();
		if(drawBlobs) blobTracker.draw(0, 0);
		
		
		glBegin(GL_LINE_LOOP);
		for(int i = 0; i < NUM_MASK_POINTS; i++) {
			glVertex2f(mask[i].x, mask[i].y);
		}
		glEnd();
		for(int i = 0; i < NUM_MASK_POINTS; i++) {
			
			ofFill();
			if(dragger==&mask[i]) glColor4f(0, 1, 0, 0.5);
			else glColor4f(1, 0, 0, 0.5);
			ofRect(mask[i].x-2, mask[i].y-2, 4, 4);
			
			ofNoFill();
			if(dragger==&mask[i]) glColor4f(0, 1, 0, 1);
			else glColor4f(1, 0, 0, 1);
			ofRect(mask[i].x-2, mask[i].y-2, 4, 4);
			
		}

		ofFill();

		for(map<int,Blob>::iterator it = blobs.begin();
			it != blobs.end(); 
			it++) {
			
			ofVec3f pos = (*it).second * ofVec3f(KINECT_WIDTH, KINECT_HEIGHT);
			ofNoFill();
			ofSetHexColor(0x0000FF);
			ofCircle(pos, 8);
			ofSetHexColor(0x009900);
			ofLine(pos.x - 5, pos.y - 5, pos.x + 5, pos.y + 5);
			ofLine(pos.x - 5, pos.y + 5, pos.x + 5, pos.y - 5);
			ofFill();
			ofSetHexColor(0x009900);
			if(rawBlobs.find((*it).first)!=rawBlobs.end()) {
				ofLine(pos.x, pos.y, rawBlobs[(*it).first].x,rawBlobs[(*it).first].y);

				ofCircle(rawBlobs[(*it).first].x,rawBlobs[(*it).first].y, 5);
			}
//				ofEllipse((*it).second.x, (*it).second.y, 10.f/640.f, 10.f/480.f);
		}
		
	}
	
	glPopMatrix();
	
	
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key=='f') {
		isFullscreen ^= true;
		ofSetFullscreen(isFullscreen);
		gui.setEnabled(!isFullscreen);
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	ofVec2f mouse;
	if(isFullscreen) {
		mouse = ofVec2f(x*KINECT_WIDTH/ofGetWidth(), y*KINECT_HEIGHT/ofGetHeight());
	} else {
		mouse = ofVec2f(x-KINECT_VIEW_OFFSET, y-KINECT_VIEW_OFFSET);
	}
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
	ofVec2f mouse;
	if(isFullscreen) {
		mouse = ofVec2f(x*KINECT_WIDTH/ofGetWidth(), y*KINECT_HEIGHT/ofGetHeight());
	} else {
		mouse = ofVec2f(x-KINECT_VIEW_OFFSET, y-KINECT_VIEW_OFFSET);
	}
	if(dragger!=NULL) {
		dragger->x = mouse.x;
		dragger->y = mouse.y;
		if(dragger->x<0) dragger->x = 0;
		if(dragger->y<0) dragger->y = 0;
		if(dragger->x>KINECT_WIDTH) dragger->x = KINECT_WIDTH;
		if(dragger->y>KINECT_HEIGHT) dragger->y = KINECT_HEIGHT;
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	ofVec2f mouse;
	if(isFullscreen) {
		mouse = ofVec2f(x*KINECT_WIDTH/ofGetWidth(), y*KINECT_HEIGHT/ofGetHeight());
	} else {
		mouse = ofVec2f(x-KINECT_VIEW_OFFSET, y-KINECT_VIEW_OFFSET);
	}
	if(dragger!=NULL) {
		dragger->x = mouse.x;
		dragger->y = mouse.y;
		if(dragger->x<0) dragger->x = 0;
		if(dragger->y<0) dragger->y = 0;
		if(dragger->x>KINECT_WIDTH) dragger->x = KINECT_WIDTH;
		if(dragger->y>KINECT_HEIGHT) dragger->y = KINECT_HEIGHT;
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

