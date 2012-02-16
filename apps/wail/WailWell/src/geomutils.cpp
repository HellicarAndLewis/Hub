#include "geomutils.h"
#include "ofVec2f.h"


float tricks::math::loopSign(ofVec2f p1, ofVec2f p2, ofVec2f p3)
{
	return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

bool tricks::math::pointIsInsideTriangle(ofVec2f pt, ofVec2f v1, ofVec2f v2, ofVec2f v3)
{
	bool b1, b2, b3;
	
	b1 = tricks::math::loopSign(pt, v1, v2) < 0.0f;
	b2 = tricks::math::loopSign(pt, v2, v3) < 0.0f;
	b3 = tricks::math::loopSign(pt, v3, v1) < 0.0f;
	
	return ((b1 == b2) && (b2 == b3));
}

ofPoint tricks::math::getIntersection(ofPoint a1, ofPoint a2, ofPoint b1, ofPoint b2) {

	// the 2 line equations
	float A1 = a2.y - a1.y;
	float B1 = a1.x - a2.x;
	float C1 = A1*a1.x + B1*a1.y;
	
	float A2 = b2.y - b1.y;
	float B2 = b1.x - b2.x;
	float C2 = A2*b1.x + B2*b1.y;
	
	float det = A1*B2 - A2*B1;
	if(det==0) {
		printf("Error!! Lines are parallel\n");
		return ofPoint(0, 0);
	} else {
		return ofPoint((B2*C1 - B1*C2)/det, (A1*C2 - A2*C1)/det);
	}
}


float tricks::math::whichSideOfLineIsPoint(const ofVec2f &a, const ofVec2f &b, const ofVec2f &p) {
	return ((a.y - b.y)*p.x + (b.x - a.x)*p.y + (a.x*b.y - b.x*a.y));
}
float tricks::math::pointLineDistance(const ofVec2f &a, const ofVec2f &b, const ofVec2f &p) {
	
	// from http://www.softsurfer.com/Archive/algorithm_0102/algorithm_0102.htm#Distance to 2-Point Line
	return ABS(((a.y - b.y)*p.x + (b.x - a.x)*p.y + (a.x*b.y - b.x*a.y))/
	sqrt((b.x-a.x)*(b.x-a.x) + (b.y - a.y)*(b.y - a.y))
	);
}



float tricks::math::angleBetweenPoints(ofVec2f a, ofVec2f b, ofVec2f c) {
	a -= b;
	c -= b;
	return a.angle(c);
	
}



void tricks::math::modifyRect(ofRectangle &rect, ofVec2f point) {
	
	// deal with the x first
	if(point.x<rect.x) {
		float diff = rect.x - point.x;
		rect.x -= diff;
		rect.width += diff;
	} else if(point.x>rect.x+rect.width) {
		float diff = point.x - (rect.x+rect.width);
		rect.width += diff;
	}
	// then the y
	if(point.y<rect.y) {
		float diff = rect.y - point.y;
		rect.y -= diff;
		rect.height += diff;
	} else if(point.y>rect.y+rect.height) {
		float diff = point.y - (rect.y+rect.height);
		rect.height += diff;
	}
	
	

}

/**
 * Gets the opengl modelview matrix
 */
ofMatrix4x4 tricks::math::getModelViewMatrix() {
	ofMatrix4x4 modelView;
	float mat[ 16 ]; 
	glGetFloatv( GL_MODELVIEW_MATRIX, mat );
	modelView.set(mat);
	return modelView;

}

/**
 * Gets the opengl projection matrix.
 */
ofMatrix4x4 tricks::math::getProjectionMatrix() {

	ofMatrix4x4 projection;
	float mat[ 16 ]; 
	glGetFloatv( GL_PROJECTION_MATRIX, mat );
	projection.set(mat);
	return projection;
}


ofRectangle tricks::math::combineRects(const ofRectangle &a, const ofRectangle &b) {
	ofVec2f minPt(FLT_MAX, FLT_MAX);
	ofVec2f maxPt(FLT_MIN, FLT_MIN);
	if(a.x<minPt.x) minPt.x = a.x;
	if(b.x<minPt.x) minPt.x = b.x;
	if(a.y<minPt.y) minPt.y = a.y;
	if(b.y<minPt.y) minPt.y = b.y;
	
	if(a.x+a.width>maxPt.x) maxPt.x = a.x+a.width;
	if(b.x+b.width>maxPt.x) maxPt.x = b.x+b.width;
	if(a.y+a.height>maxPt.y) maxPt.y = a.y+a.height;
	if(b.y+b.height>maxPt.y) maxPt.y = b.y+b.height;
	
	return ofRectangle(minPt.x, minPt.y, maxPt.x - minPt.x, maxPt.y - minPt.y);
}


ofRectangle tricks::math::rotateRect(ofRectangle &r, float rotation) {
	ofVec2f corners[4];
	corners[0] = ofVec2f(r.x, r.y);
	corners[1] = ofVec2f(r.x+r.width, r.y);
	corners[2] = ofVec2f(r.x+r.width, r.y+r.height);
	corners[3] = ofVec2f(r.x, r.y+r.height);
	ofVec2f minP(FLT_MAX,FLT_MAX);
	ofVec2f maxP(FLT_MIN, FLT_MIN);
	for(int i = 0; i < 4; i++) {
		corners[i].rotate(rotation);
		if(corners[i].x<minP.x) minP.x = corners[i].x;
		if(corners[i].x>maxP.x) maxP.x = corners[i].x;
		if(corners[i].y<minP.y) minP.y = corners[i].y;
		if(corners[i].y>maxP.y) maxP.y = corners[i].y;
	}
	return ofRectangle(minP.x, minP.y, maxP.x - minP.x, maxP.y - minP.y);
	
}