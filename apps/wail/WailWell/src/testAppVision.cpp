/**
 *  testAppVision.cpp
 *
 *  Created by Marek Bereza on 06/02/2012.
 */

#include "testApp.h"
#include "contourutils.h"
#include "geomutils.h"



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
	return (blob.boundingRect.width/VISION_WIDTH + blob.boundingRect.height/VISION_HEIGHT)*0.5;
}

void testApp::normalizeBlobCoords(ofVec3f &blob) {
	blob.x /= VISION_WIDTH;
	blob.y /= VISION_HEIGHT;
	
	// x and y now need to be tapered to work with the frustrum of the camera
	// this is done by feel with sliders in the gui.
	
	float scaleFactorX = ofMap(blob.z, 0, 1, xScaleTop, xScaleBottom);
	float scaleFactorY = ofMap(blob.z, 0, 1, yScaleTop, yScaleBottom);
	ofVec2f normCentre = centre/ofVec2f(VISION_WIDTH, VISION_HEIGHT);
	// scale out from the centre
	ofVec2f newCoord = ofVec2f(blob.x, blob.y) - normCentre;
	newCoord *= ofVec2f(scaleFactorX, scaleFactorY);
	
	blob.x =  normCentre.x + newCoord.x;//*(flipX?-1:1);
	blob.y =  normCentre.y+ newCoord.y;//*(flipY?-1:1);
	// z is already scaled
	
	// clamp em
	blob.x = ofClamp(blob.x, 0, 1);
	blob.y = ofClamp(blob.y, 0, 1);
	
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

void mergeBlobIntoBlob(ofxCvBlob &blob, const ofxCvBlob &dinner) {
	// merge points
	for(int i = 0; i < dinner.nPts; i++) {
		blob.pts.push_back(dinner.pts[i]);
	}
	// merge boundingRects
	blob.boundingRect = tricks::math::combineRects(blob.boundingRect, dinner.boundingRect);
	
	// adjust centroid
	blob.centroid = blob.boundingRect.getCenter();
}

void testApp::doVision() {
	float startTime = ofGetElapsedTimef();
	float img[VISION_WIDTH*VISION_HEIGHT];
	float *distPix = kinect.getDistancePixels();
	
	
	float kinectRangeFar = 4000;
	float kinectRangeNear = 500;
	
	
	float denom = (kinectRangeFar - kinectRangeNear) * (maxWaterDepth - waterThreshold);
	if(denom==0) denom = 0.0001;
	float subtractor = kinectRangeFar - waterThreshold*(kinectRangeFar - kinectRangeNear);
	
	for(int i = 0; i < VISION_WIDTH*VISION_HEIGHT; i++) {
		
		int x = i % VISION_WIDTH;
		int y = i / VISION_WIDTH;
		
		int ii = x*2 + y * 2 * KINECT_WIDTH;
		
		// this is slow
		//img[i] = ofMap(ofMap(distPix[i], 500, 4000, 1, 0), maxWaterDepth, waterThreshold, 1, 0);
		
		img[i] = (subtractor - distPix[ii])/denom;
		if(img[i]>1) img[i] = 0;
		if(img[i]<0) img[i] = 0;
	}
	depthImg.setFromPixels(img,VISION_WIDTH,VISION_HEIGHT);
	
	if(flipX || flipY) depthImg.mirror(flipY, flipX);
	
	rangeScaledImg = depthImg;
	
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
	
	int numPix = VISION_WIDTH * VISION_HEIGHT;
	
	for(int i = 0; i < numPix; i++) {
		if(fgPix[i]<=bgPix[i]+backgroundHysteresis) {
			fgPix[i] = 0;
		}
	}
	
	rangeScaledImg.setFromPixels(fgPix, VISION_WIDTH, VISION_HEIGHT);
	
	
	maskedImg = rangeScaledImg;
	
	CvPoint points[NUM_MASK_POINTS];
	for(int i = 0; i < NUM_MASK_POINTS; i++) {
		points[i] = cvPoint(mask[i].x, mask[i].y);
	}
	
	
	CvPoint fill[4];
	fill[0] = cvPoint(0, 0);
	fill[1] = cvPoint(VISION_WIDTH, 0);
	fill[2] = cvPoint(VISION_WIDTH, VISION_HEIGHT);
	fill[3] = cvPoint(0, VISION_HEIGHT);
	
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
				if(dist<20) {
					float distToI = FLT_MAX;
					float distToJ = FLT_MAX;
					for(map<int,Blob>::iterator it = blobs.begin(); it!=blobs.end(); it++) {
						
						distToI = MIN(distToI, (*it).second.distanceSquared(ofVec2f(contourFinder.blobs[i].centroid)));
						distToJ = MIN(distToJ, (*it).second.distanceSquared(ofVec2f(contourFinder.blobs[j].centroid)));
					}
					if(distToI<distToJ) {
						// merge the jth into the ith, and delete the jth one
						mergeBlobIntoBlob(contourFinder.blobs[i], contourFinder.blobs[j]);
						contourFinder.blobs.erase(contourFinder.blobs.begin() + j);
						j--;
					} else {
						
						// merge the ith into the jth, and delete the ith one
						mergeBlobIntoBlob(contourFinder.blobs[j], contourFinder.blobs[i]);
						contourFinder.blobs.erase(contourFinder.blobs.begin() + i);
						i--;
						break;
					}
				}
			}
		}
		
	}
	blobTracker.trackBlobs(contourFinder.blobs);
	
	
}

void testApp::drawKinect() {
	if(viewMode==VIEWMODE_RAW) {
		depthImg.draw(0, 0, VISION_WIDTH, VISION_HEIGHT);
	} else if(viewMode==VIEWMODE_BG) {
		bgImg.draw(0, 0, VISION_WIDTH, VISION_HEIGHT);
	} else if(viewMode==VIEWMODE_RANGE_SCALE) {
		rangeScaledImg.draw(0, 0, VISION_WIDTH, VISION_HEIGHT);
	} else if(viewMode==VIEWMODE_MASKED) {
		maskedImg.draw(0, 0, VISION_WIDTH, VISION_HEIGHT);
	} else if(viewMode==VIEWMODE_MUTE) {
		// nothing
	}
}