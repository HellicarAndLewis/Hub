#include "testApp.h"
#include "ofxXmlSettings.h"
#include "contourutils.h"

//
//
// NOTE FROM MAREK - 
// 
// I made a new project called "WailWall" which is for the visualization.
//
// This project is just for the kinect reading, and will have OSC going out
// to the "WailWall" app.
//

enum {
	VIEWMODE_RAW,
	VIEWMODE_BG,
	VIEWMODE_RANGE_SCALE,
	VIEWMODE_MASKED,
	VIEWMODE_MUTE
};

int KINECT_WIDTH;
int KINECT_HEIGHT;
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
	
	viewMode = VIEWMODE_RAW;
	wallOsc.setup("localhost", 1234);
	soundOsc.setup("localhost", 2468);
	blurSize = 0;
	blurIterations = 0;
	waterThreshold = 10;
	maxWaterDepth = 255;
	minBlobSize = 10;
	maxBlobSize = 200;


	accumulateBackground = false;
	backgroundAccumulationCount = 0;
	
	drawBlobs = true;
	erode = false;
	dilate = false;
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
	
	blobTracker.setListener(this);
	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	
	setupMask();
	
	
	//gui.setup();
	//gui.addDrawable("Depth", depthImg);
	gui.setWidth(206);
	gui.addSegmented("View", viewMode, "RAW|BACKGROUND|RANGE SCALE|MASKED|MUTE");
	gui.addSlider("Blur Size", blurSize, 0, 3)->stepped = true;
	gui.addSlider("Blur Iterations", blurIterations, 0, 3)->stepped = true;
	gui.addToggle("Erode Image", erode);
	gui.addToggle("Dilate Image", dilate);
	gui.addPushButton("Accumulate Background")->width = 150;
	gui.addSlider("Background Hysteresis", backgroundHysteresis, 0, 0.1);
	gui.addColumn();
	gui.addToggle("Draw Blobs", drawBlobs);
	gui.addSlider("Water Surface Level", waterThreshold, 0, 1);//->stepped = true;
	gui.addSlider("Maximum Water Depth", maxWaterDepth, 0, 1);//->stepped = true;
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

float lastVisionCalculationDuration = 0;

//--------------------------------------------------------------
void testApp::update(){
	kinect.update();
	if(kinect.isFrameNew()) {
		float startTime = ofGetElapsedTimef();
		float img[KINECT_WIDTH*KINECT_HEIGHT];
		float *distPix = kinect.getDistancePixels();
		for(int i = 0; i < KINECT_WIDTH*KINECT_HEIGHT; i++) {
			img[i] = ofMap(ofMap(distPix[i], 500, 4000, 1, 0), maxWaterDepth, waterThreshold, 1, 0);
			if(img[i]>1) img[i] = 0;
			if(img[i]<0) img[i] = 0;
		}
		depthImg.setFromPixels(img,KINECT_WIDTH,KINECT_HEIGHT);
		
		
		rangeScaledImg = depthImg;
		/*// threshold anything too close to the camera
		cvThreshold(rangeScaledImg.getCvImage(), rangeScaledImg.getCvImage(), maxWaterDepth, 1, CV_THRESH_TOZERO_INV);
		rangeScaledImg.flagImageChanged();
		
		// threshold anything too far away from the camera - i.e. the surface of the water
		cvThreshold(rangeScaledImg.getCvImage(), rangeScaledImg.getCvImage(), waterThreshold, 1, CV_THRESH_TOZERO);
		rangeScaledImg.flagImageChanged();
		
		float scale = 1/(maxWaterDepth-waterThreshold);
		cvConvertScale( rangeScaledImg.getCvImage(), rangeScaledImg.getCvImage(), scale, -(waterThreshold*scale));
	
		rangeScaledImg.flagImageChanged();
		*/
		
		for(int i = 0; i < blurIterations; i++) {
			rangeScaledImg.blur(2*blurSize+1);
		}
		if(erode) {
			rangeScaledImg.erode();
		}
		if(dilate) {
			rangeScaledImg.dilate();
		}
		
		
		
		if(accumulateBackground) {
			cvMax(bgImg.getCvImage(), rangeScaledImg.getCvImage(), bgImg.getCvImage());
			bgImg.flagImageChanged();
			
			
			backgroundAccumulationCount++;
			if(backgroundAccumulationCount>100) {
				accumulateBackground = false;
				backgroundAccumulationCount = 0;
			}
		}
		
		
		float *fgPix = rangeScaledImg.getPixelsAsFloats();
		float *bgPix = bgImg.getPixelsAsFloats();
		
		int numPix = KINECT_WIDTH * KINECT_HEIGHT;
		
		for(int i = 0; i < numPix; i++) {
			if(fgPix[i]<=bgPix[i]+backgroundHysteresis) {
				fgPix[i] = 0;
			}
		}
		
		rangeScaledImg.setFromPixels(fgPix, KINECT_WIDTH, KINECT_HEIGHT);
	
		
		maskedImg = rangeScaledImg;
		
		CvPoint points[NUM_MASK_POINTS];
		for(int i = 0; i < NUM_MASK_POINTS; i++) {
			points[i] = cvPoint(mask[i].x, mask[i].y);
		}
		
		
		CvPoint fill[4];
		fill[0] = cvPoint(0, 0);
		fill[1] = cvPoint(KINECT_WIDTH, 0);
		fill[2] = cvPoint(KINECT_WIDTH, KINECT_HEIGHT);
		fill[3] = cvPoint(0, KINECT_HEIGHT);
		
		CvPoint *allPoints[2];
		allPoints[0] = points;
		allPoints[1] = fill;
		
		int numPoints[2];
		numPoints[0] = NUM_MASK_POINTS;
		numPoints[1] = 4;
		
		// mask out the bit we're interested in
		cvFillPoly(
				   maskedImg.getCvImage(), allPoints,
				   numPoints,
				   2, cvScalar(0)
				   );
		maskedImg.flagImageChanged();
		
		contourFinder.findContours(maskedImg, minBlobSize*minBlobSize, maxBlobSize*maxBlobSize, 20, false);
		

		// compare each blob against the other and eradicate any blobs that are too close to eachother
		if(blobs.size()>0) {
			for(int i = 0; i < contourFinder.blobs.size(); i++) {
				for(int j = i+1; j < contourFinder.blobs.size(); j++) {
					float dist = tricks::math::getClosestDistanceBetweenTwoContours(
												contourFinder.blobs[i].pts, 
												contourFinder.blobs[j].pts, 3);

					// find which one is closest to any other blob and delete the other one
					if(dist<10) {
						float distToI = FLT_MAX;
						float distToJ = FLT_MAX;
						for(map<int,Blob>::iterator it = blobs.begin(); it!=blobs.end(); it++) {
							
							distToI = MIN(distToI, (*it).second.distanceSquared(ofVec2f(contourFinder.blobs[i].centroid)));
							distToJ = MIN(distToJ, (*it).second.distanceSquared(ofVec2f(contourFinder.blobs[j].centroid)));
						}
						if(distToI<distToJ) {
							// delete the jth one
							contourFinder.blobs.erase(contourFinder.blobs.begin() + j);
							j--;
						} else {
							// delete the ith one
							contourFinder.blobs.erase(contourFinder.blobs.begin() + i);
							i--;
							break;
						}
					}
				}
			}
		 
		}
		blobTracker.trackBlobs(contourFinder.blobs);
	
	 lastVisionCalculationDuration = ofGetElapsedTimef() - startTime;
		 
	}
	
	
	ofSetWindowTitle("WailWell - "+ ofToString(ofGetFrameRate(), 2) + "fps - " + ofToString(lastVisionCalculationDuration*1000, 0) + "ms");
	
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

		if(viewMode==VIEWMODE_RAW) {
			depthImg.draw(0, 0, KINECT_WIDTH, KINECT_HEIGHT);
		} else if(viewMode==VIEWMODE_BG) {
			bgImg.draw(0, 0, KINECT_WIDTH, KINECT_HEIGHT);
		} else if(viewMode==VIEWMODE_RANGE_SCALE) {
			rangeScaledImg.draw(0, 0, KINECT_WIDTH, KINECT_HEIGHT);
		} else if(viewMode==VIEWMODE_MASKED) {
			maskedImg.draw(0, 0, KINECT_WIDTH, KINECT_HEIGHT);
		} else if(viewMode==VIEWMODE_MUTE) {
			// nothing
		}
		
		//contourFinder.draw(0, 0);
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
	


		//glPushMatrix();
		//{
		//	glScalef(640, 480, 1);
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
		//}
		//glPopMatrix();
	
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


ofVec3f testApp::getBlobCoords(ofxCvTrackedBlob &blob) {
	double minVal;
	double maxVal;
	CvPoint maxLoc;

	
	maskedImg.setROI(blob.boundingRect);
	cvMinMaxLoc(maskedImg.getCvImage(),
				&minVal, &maxVal, NULL, &maxLoc);
	maskedImg.resetROI();
	
	

	ofVec3f coords = ofVec3f((float)(blob.boundingRect.x + maxLoc.x), (float)(blob.boundingRect.y + maxLoc.y), (float)maxVal);
	
	// take an average of the centroid and the deepest point (only in x and y axes)
	// - this should provide some smoothing.
	coords.x = (coords.x + blob.centroid.x)/2;
	coords.y = (coords.y + blob.centroid.y)/2;
	
	return coords;
	
}


float testApp::getBlobSize(ofxCvTrackedBlob &blob) {
	return (blob.boundingRect.width/KINECT_WIDTH + blob.boundingRect.height/KINECT_HEIGHT)*0.5;
}

void testApp::normalizeBlobCoords(ofVec3f &blob) {
	blob.x /= KINECT_WIDTH;
	blob.y /= KINECT_HEIGHT;
	
	// x and y now need to be tapered to work with the frustrum of the camera
	// this is done by feel with sliders in the gui.
	
	float scaleFactorX = ofMap(blob.z, 0, 1, xScaleTop, xScaleBottom);
	float scaleFactorY = ofMap(blob.z, 0, 1, yScaleTop, yScaleBottom);
	
	// scale out from the centre
	ofVec2f newCoord = ofVec2f(blob.x, blob.y) - ofVec2f(0.5, 0.5);
	newCoord *= ofVec2f(scaleFactorX, scaleFactorY);
	
	blob.x =  0.5 + newCoord.x*(flipX?-1:1);
	blob.y =  0.5 + newCoord.y*(flipY?-1:1);
	
	// z is already scaled
}


// callbacks for blob listener
void testApp::blobOn( int x, int y, int id, int order ) {
	ofxCvTrackedBlob &b = blobTracker.getById(id);
	
	ofVec3f blobCoords = getBlobCoords(b);
	rawBlobs[id] = blobCoords;
	
	normalizeBlobCoords(blobCoords);
	
	blobs[id] = Blob(blobCoords);

	ofxOscMessage msg;
	msg.setAddress("/touch/down");
	msg.addIntArg(id);
	msg.addFloatArg(blobs[id].x);
	msg.addFloatArg(blobs[id].y);
	msg.addFloatArg(blobs[id].z);
	msg.addFloatArg(getBlobSize(b));
	wallOsc.sendMessage(msg);
	soundOsc.sendMessage(msg);
}

void testApp::blobMoved( int x, int y, int id, int order ) {
	ofxCvTrackedBlob &b = blobTracker.getById(id);
	ofVec3f blobCoords = getBlobCoords(b);
	rawBlobs[id] = blobCoords;
	
	normalizeBlobCoords(blobCoords);
	if(blobs.find(id)!=blobs.end()) {
		blobs[id].update(blobCoords);
	} else {
		// if we don't already have this blob 
		// (i.e. if the blobtracker has a bug)
		// send the blob to blobOn and get out
		blobOn(x, y, id, order);
		return;
	}
	ofxOscMessage msg;
	msg.setAddress("/touch/moved");
	msg.addIntArg(id);
	msg.addFloatArg(blobs[id].x);
	msg.addFloatArg(blobs[id].y);
	msg.addFloatArg(blobs[id].z);
	msg.addFloatArg(getBlobSize(b));
	wallOsc.sendMessage(msg);
	soundOsc.sendMessage(msg);
}
void testApp::blobOff( int x, int y, int id, int order ) {
	if(blobs.find(id)!=blobs.end()) {
		blobs.erase(id);
	}
	if(rawBlobs.find(id)!=rawBlobs.end()) {
		rawBlobs.erase(id);
	}
	
	ofxOscMessage msg;
	msg.setAddress("/touch/up");
	msg.addIntArg(id);
	wallOsc.sendMessage(msg);
	soundOsc.sendMessage(msg);
}