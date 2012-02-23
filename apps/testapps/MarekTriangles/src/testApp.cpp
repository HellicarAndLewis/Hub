#include "testApp.h"
#include "ofxDelaunay.h"
#include "Caustics.h"

class Particle: public ofVec2f {
public:
	ofVec2f vel;
	
	
	Particle() {
		x = ofRandomWidth();
		y = ofRandomHeight();
		vel.set(ofRandom(0.3, 2), 0);
		vel.rotate(ofRandom(0, 360));
	}
	
	
	void attract(const ofVec2f &p) {
		ofVec2f delta = p - *this;
		float dist = delta.length();
		delta /= dist;
		vel += delta/15;
	}
	void update() {
		*this += vel;
		
		vel *= 0.99;
		if(x<0) {
			x = 0;
			vel.x *= -1;
		}
		
		if(y<0) {
			y = 0;
			vel.y *= -1;
		}
		
		if(x>ofGetWidth()) {
			x = ofGetWidth();
			vel.x *= -1;
		}
		
		if(y>ofGetHeight()) {
			y = ofGetHeight();
			vel.y *= -1;
		}
	}
	void draw() {
		ofCircle(*this, 3);
	}
	

};

vector<Particle> particles;

bool mouseIsDown = false;
Caustics caustics;
//--------------------------------------------------------------
void testApp::setup(){
	
	for(int i =0; i < 200; i++) {
		particles.push_back(Particle());
	}
	
	caustics.setup(ofGetWidth(), ofGetHeight(), 200);
	ofSetFrameRate(60);
	ofBackground(0);
	//ofSetBackgroundAuto(false);
	
}

//--------------------------------------------------------------
void testApp::update(){
	caustics.reset();
	if(mouseIsDown) {
		ofVec2f mouse(mouseX, mouseY);
		for(int i = 0; i < particles.size(); i++) {
			particles[i].attract(mouse);
		}
	}
	for(int i = 0; i < particles.size(); i++) {
		particles[i].update();
		caustics.addPoint(particles[i]);
	}

	caustics.update();
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 0));
}

//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	caustics.renderToFbo();
	caustics.getFbo().draw(0, 0);
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
	mouseIsDown = true;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	mouseIsDown = false;
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