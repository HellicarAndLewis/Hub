#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(22,33,44);
	glGenBuffers(1, &pbo);
	
	int w = ofGetWidth();
	int h = ofGetHeight();
	int size = w * h * 4;
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
	glBufferData(GL_PIXEL_PACK_BUFFER, size, NULL, GL_STATIC_READ);

}



//--------------------------------------------------------------
void testApp::draw(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, 4.0/3.0, 0.1, 100);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0,0,-10);
	
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
	
	glColor3f(1,0,0.5);
	float s = 3;
	glBegin(GL_QUADS);
		glVertex3f(-s, s, 0);
		glVertex3f(s,s,0);
		glVertex3f(s,-s,0);
		glVertex3f(-s, -s, 0);
	glEnd();
	
	// the last 0 at the end is the offset in the buffer we want to read, this
	// is because a pixel buffer is bound; 
	// so the way glReadPixels works is changed when binding the
	// pixel buffer.
	glReadPixels(0, 0, ofGetWidth(), ofGetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, 0);
	
	GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
	if(ptr) {
		ofPixels pix;
		ofImage img;
		pix.setFromPixels(ptr, ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);
		img.setFromPixels(pix);
		img.saveImage(ofGetTimestampString() +".png");
		//ofSaveImage(pix, "test.jpg");
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
	}
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

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