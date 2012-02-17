#include "testApp.h"
//#undef eglGetError
//	#define eglGetError()



//--------------------------------------------------------------
void testApp::setup(){
	glEnable(GL_TEXTURE_2D); eglGetError();
	glGenTextures(1, &tex); eglGetError();
	glBindTexture(GL_TEXTURE_2D, tex); eglGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  eglGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); eglGetError(); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); eglGetError();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); eglGetError(); 
	
	tex_img.loadImage("nice.jpg"); eglGetError();
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA8, tex_img.getWidth(), tex_img.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, tex_img.getPixels()); eglGetError();
	ofBackground(22,33,44);
	
	
	
	int w = tex_img.getWidth();
	int h = tex_img.getHeight();
	int size = w * h * 4;
	glGenBuffers(1, &pbo); eglGetError();
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo); eglGetError();
	glBufferData(GL_PIXEL_PACK_BUFFER, size, NULL, GL_STATIC_READ); eglGetError();
	save = false;
	mapped = false;
	//glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}



//--------------------------------------------------------------
void testApp::draw(){
	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION); eglGetError();
	glLoadIdentity(); eglGetError();
	gluPerspective(45.0f, 4.0/3.0, 0.1, 100); eglGetError();
	
	glMatrixMode(GL_MODELVIEW); eglGetError();
	glLoadIdentity(); eglGetError();
	glTranslatef(0,0,-10); eglGetError();
	

	glColor3f(1,1,1); eglGetError();
	float s = 3;
	glBegin(GL_QUADS); 
		glTexCoord2f(0.0, 0.0); glVertex3f(-s, s, 0); 
		glTexCoord2f(1.0, 0.0); glVertex3f(s,s,0); 
		glTexCoord2f(1.0, 1.0);	glVertex3f(s,-s,0); 
		glTexCoord2f(0.0, 1.0);	glVertex3f(-s, -s, 0);
	glEnd(); eglGetError();
	
	//glBindTexture(GL_TEXTURE_2D, tex); eglGetError();	
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo); eglGetError();

	
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, 0); eglGetError();
	GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY); eglGetError();
	
	if(ptr) {
		if(save) {
			save = false;
			ofPixels pix; eglGetError();
			ofImage img; eglGetError();
			pix.setFromPixels(ptr, tex_img.getWidth(), tex_img.getHeight(), OF_IMAGE_COLOR); eglGetError();
			img.setFromPixels(pix); eglGetError();
			img.saveImage(ofGetTimestampString() +".jpg"); eglGetError();
		}
	}
	else {
	}
	
	glUnmapBuffer(GL_PIXEL_PACK_BUFFER); eglGetError();
	
}


/*


	
		
	//glBindTexture(GL_TEXTURE_2D, tex); eglGetError();	
	//	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo); eglGetError();

	
	
	
	// the last 0 at the end is the offset in the buffer we want to read, this
	// is because a pixel buffer is bound; 
	// so the way glReadPixels works is changed when binding the
	// pixel buffer.
	//glReadPixels(0, 0, tex_img.getWidth(), tex_img.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, 0); eglGetError();
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA8, GL_UNSIGNED_BYTE, 0); eglGetError();
	GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY); eglGetError();
	if(ptr && save) {
		printf("SAVE!\n");
		save = false;
//		ofPixels pix; eglGetError();
//		ofImage img; eglGetError();
//		pix.setFromPixels(ptr, tex_img.getWidth(), tex_img.getHeight(), OF_IMAGE_COLOR_ALPHA); eglGetError();
//		img.setFromPixels(pix); eglGetError();
//		img.saveImage(ofGetTimestampString() +".png"); eglGetError();
//		//ofSaveImage(pix, "test.jpg");
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER); eglGetError();
	}
	else {
		printf("Error getting data ptr.\n");
	}

*/
//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if(key == ' ') {
		save = true;
		printf("save!\n");
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