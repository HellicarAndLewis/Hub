#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetFrameRate(30);

	
	drop.load("dropshader");
	updater.load("updateshader");
	normals.load("updatenormals");
	caustics.load("caustics");
	
	
	caustics.begin();
	caustics.setUniform1f("water", 0);
	caustics.end();
	
	updater.begin();
	updater.setUniform1f("texture", 0);
	updater.end();
	
	normals.begin();
	normals.setUniform1f("texture", 0);
	normals.end();
	
	drop.begin();
	drop.setUniform1f("texture", 0);
	drop.end();
	
	causticTex.allocate(512, 512, GL_RGBA32F_ARB);
	causticTex.begin();
	ofClear(0);
	causticTex.end();
	
	for(int i = 0; i < 2; i++){
		waterTex[i].allocate(512, 512, GL_RGBA32F_ARB);
		waterTex[i].begin();
		ofClear(0,0,0,0);
		waterTex[i].end();
	}
	waterswapcur = 0;
	waterswapnxt = 1;
	
	addDrop(512/2, 512/2, 40, .01); 
//	addDrop(512/2, 512/2, 20, -.005);
	
//	for (int i = 0; i < 20; i++) {
//		addDrop(ofRandomuf()*512, ofRandomuf()*512, 10, (i & 1) ? -0.01 : -0.01);
//	}
	delta = 1.0;
	
}

//--------------------------------------------------------------
void testApp::update(){
	
//	for (int i = 0; i < 10; i++) {
	if(ofRandomuf() > .5)
		addDrop(ofRandomuf()*512, ofRandomuf()*512, 20, ofGetFrameNum() % 2 == 0 ? 0.02 : -0.02);
//	}
	
	
//	addDrop(ofGetMouseX(), ofGetMouseY(), 20, ofGetFrameNum() % 2 == 0 ? -.2 : .2);
	//addDrop(ofGetMouseX(), ofGetMouseY(), 10, .01);
	
	stepSimulation();
	stepSimulation();
	updateNormals();
	updateCaustics();	
}

void testApp::stepSimulation(){
	waterTex[waterswapcur].begin();
//	ofClear(0);

	updater.begin();

	updater.setUniform2f("delta", delta,delta);
	
	waterTex[waterswapnxt].draw(0, 0);
	
	updater.end();
	
	waterTex[waterswapcur].end();
	
	swapWaterTexture();	
	
}

void testApp::updateNormals(){
	waterTex[waterswapcur].begin();
//	ofClear(0);

	normals.begin();
	normals.setUniform2f("delta", delta,delta);
	
	waterTex[waterswapnxt].draw(0, 0);
	
	normals.end();
	
	waterTex[waterswapcur].end();
	
	swapWaterTexture();		
}

void testApp::updateCaustics(){
	
	causticTex.begin();
	ofClear(0);
	
	ofVec3f light(ofGetMouseX(), 0, ofGetMouseY());
	light.normalize();
	
	caustics.begin();
	caustics.setUniform3f("light", light.x, light.y, light.z);
//	caustics.setUniform3f("light", -.2, 0., 1.);
	waterTex[waterswapcur].draw(0, 0);
	caustics.end();
	
	causticTex.end();
}

void testApp::addDrop(float x, float y, float radius, float strength){
	
	waterTex[waterswapcur].begin();
//	ofClear(0);
	
	drop.begin();
	drop.setUniform2f("center", x, y);
	drop.setUniform1f("radius", radius);
	drop.setUniform1f("strength", strength);
	
	waterTex[waterswapnxt].draw(0, 0);
	
	drop.end();
	
	waterTex[waterswapcur].end();
	
	swapWaterTexture();	
}

void testApp::swapWaterTexture(){
	waterswapcur = (waterswapcur + 1) % 2;
	waterswapnxt = (waterswapnxt + 1) % 2;
}


//--------------------------------------------------------------
void testApp::draw(){
	ofBackground(0);
	waterTex[waterswapcur].draw(0,0);
	causticTex.draw(waterTex[waterswapcur].getWidth(),0);
	

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