/*
 *  ofxWWRenderer.cpp
 *  WailWell
 *
 *  Created by James George on 1/29/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxWWRenderer.h"
#include "ofxWebSimpleGuiToo.h"
#include "Colours.h"
#include "mathutils.h"


bool monday = false;
bool tuesday = false;
bool wednesday = false;
bool thursday = false;
bool friday = false;
float causticHaloRadius = 1000;
void ofxWWRenderer::setup(int width, int height){
	targetWidth = width;
	targetHeight = height;

	searchTermHaloShader.load("shaders/halo.vert", "shaders/halo.frag");
	
	causticsAlwaysOn = false;
	// max number of particles is the final arg, might need higher
	caust.setup(width, height, 5000);
	screenshotTarget.allocate(width/4, height/4, GL_RGB);
	
	
	//final buffer for comping it all together
	renderTarget.allocate(width, height, GL_RGB);
	
	gradientOverlay.allocate(width/8, height/8, GL_RGB);
    background.allocate(width/8, height/8, GL_RGB); //going to be scaled anyway, and a single colour
    
	Colours::set(SURFACE_BG, 0xffffff);
	Colours::set(SEARCH_BG, 0x000000);
	Colours::set(HALO_SURFACE, 0xffffff);
	Colours::set(HALO_SEARCH, 0xEEEEEE);
	
	layer1Target.allocate(width, height, GL_RGBA);
	layer2Target.allocate(width, height, GL_RGBA);
	
	tweets.simulationWidth = width;
	tweets.simulationHeight = height;
	
	accumulator[0].begin();
	ofClear(0);
	accumulator[0].end();
	accumulator[1].begin();
	ofClear(0);
	accumulator[1].end();
	
	tweets.fluidRef = &fluid;
	tweets.blobsRef = blobs;
	
	caustics.shaderPath = "shaders/";
	caustics.setup(width/4, height/4);
	
	colorField.loadImage("images/color_palette.png");
	fluid.sampleTexture = &colorField;
	layerOneBackgroundA.loadImage("images/BGGradientA_layer1.png");
	layerTwoBackgroundA.loadImage("images/BGGradientA_layer2.png");
	layerOneBackgroundB.loadImage("images/BGGradientB_layer1.png");
	layerTwoBackgroundB.loadImage("images/BGGradientB_layer2.png");
	
	layer1Opacity = 1.0;
	
	alphaFade.load("shaders/alphafade");
	alphaFade.begin();
	alphaFade.setUniform1i("self", 0);
	alphaFade.end();
	
	permutationImage.loadImage("shaders/permtexture.png");
	noiseShader.load("shaders/noise");
	noiseShader.begin();
	noiseShader.setUniform1i("permTexture", 0);
	noiseShader.end();
	
	blurShader.load("shaders/gaussian_blur");
	blurShader.begin();
	blurShader.setUniform1i("tex0", 0);
	blurShader.end();
	
	warpShader.load("shaders/warp_distort");
	warpShader.begin();
	warpShader.setUniform1i("base",0);
	warpShader.setUniform1i("warp",1);
	warpShader.end();

	glowShader.load("shaders/blendcaustics");
	glowShader.begin();
	glowShader.setUniform1i("base",0);
	glowShader.setUniform1i("caustics",1);	
	glowShader.end();
	
	enableFluid = false;
	justDrawWarpTexture = false;

	halo.loadImage("images/halo.png");
	
//	cout << "setting up tweets" << endl;
	tweets.setup(this);

	callToAction.setup(&tweets);
	// roxlu: test screenshots
	ofAddListener(ofEvents.keyPressed, this, &ofxWWRenderer::keyPressed);
	test_screenshot = false;
}

void ofxWWRenderer::setupGui(){
	
	webGui.addPage("Interaction");
	webGui.addSlider("Layer Barrier Z", layerBarrierZ, .25, .75);
	webGui.addSlider("Layer Barrier Width", layerBarrierWidth, 0.05, .25);
	webGui.addSlider("Touch Scale", tweets.touchSizeScale, .25, 1.5);
	webGui.addSlider("Influence Width", tweets.touchInfluenceFalloff, 200, 5000);
	webGui.addToggle("Draw Touch Debug", drawTouchDebug);
	
	
	
	webGui.addPage("Caustics");
	
	// tweakable variables
	// how much it goes down by [-2->+2]
	webGui.addSlider("Vertical Drift", caust.verticalDrift, -2, 2);
	
	// how much the zoom blur happens [0-10]
	// it doesnt make any difference to performance
	// how much you do.
	webGui.addSlider("Scale factor",  caust.scaleFactor, 0, 10);
	
	
	// how much the back fades out [0.95-1]
	webGui.addSlider("Blend",  caust.fade, 0.95, 1);
	
	
	// [0.01-0.1]
	webGui.addSlider("Brightness",  caust.brightness, 0.01, 0.1);
	
	webGui.addSlider("Oscillation",  caust.oscillation, 0, 20);
	webGui.addToggle("Always on", causticsAlwaysOn);
	
	webGui.addSlider("caustic halo radius", causticHaloRadius, 200, 3500);    
	
	
	
    webGui.addPage("Colours");

    webGui.addHexColor("Surface Background", Colours::get(SURFACE_BG));
    webGui.addHexColor("Search Background", Colours::get(SEARCH_BG));


    webGui.addHexColor("Halo Surface", Colours::get(HALO_SURFACE));
    webGui.addHexColor("Halo Bottom", Colours::get(HALO_SEARCH));    

	webGui.addHexColor("At Sign Color", Colours::get(AT_SIGN));
	webGui.addHexColor("Layer One Font Color", Colours::get(LAYER_1_FONT));
	webGui.addHexColor("Layer Two Font Color", Colours::get(LAYER_2_FONT));
	
	webGui.addHexColor("Tweet Color", tweets.particleHexImageColor);
	
	webGui.addToggle("Monday", monday);
	webGui.addToggle("Tuesday", tuesday);
	webGui.addToggle("Wednesday", wednesday);
	webGui.addToggle("Thursday", thursday);
	webGui.addToggle("Friday", friday);
	
	
	tweets.setupGui();
}

void ofxWWRenderer::update(){
	if(monday) {
		monday = false;
		Colours::set(AT_SIGN,		0xf9b40f);
		Colours::set(SURFACE_BG,	0x7a1600);
		Colours::set(HALO_SEARCH,	0xe2ded9);
		Colours::set(HALO_SURFACE,	0xe2ded9);
		Colours::set(SEARCH_BG,		0x3b0608);
	}
	if(tuesday) {
		tuesday = false;
		Colours::set(AT_SIGN,		0xf9b40f);
		Colours::set(SURFACE_BG,	0x002d56);
		Colours::set(HALO_SEARCH,	0xf9b40f);
		Colours::set(HALO_SURFACE,	0xf9b40f);
		Colours::set(SEARCH_BG,		0x000c2e);
	}
	
	if(wednesday) {
		wednesday = false;
		Colours::set(AT_SIGN,		0xfdf3e4);
		Colours::set(SURFACE_BG,	0xe86d1f);
		Colours::set(HALO_SEARCH,	0xfdf3e4);
		Colours::set(HALO_SURFACE,	0xfdf3e4);
		Colours::set(SEARCH_BG,		0x571c1f);
	}

	
	if(thursday) {
		thursday = false;
		Colours::set(AT_SIGN,		0x002d56);
		Colours::set(SURFACE_BG,	0xfaa634);
		Colours::set(HALO_SEARCH,	0xFFFFFF);
		Colours::set(HALO_SURFACE,	0xFFFFFF);
		Colours::set(SEARCH_BG,		0xa84d10);
	}
	
	if(friday) {
		friday = false;
		Colours::set(AT_SIGN,		0xe86d1f);
		Colours::set(SURFACE_BG,	0xd1c8b3);
		Colours::set(HALO_SEARCH,	0xd1c8b3);
		Colours::set(HALO_SURFACE,	0xd1c8b3);
		Colours::set(SEARCH_BG,		0x00334e);
	}

	float maxTouchZ = 0;
	map<int,KinectTouch>::iterator it;
	for(it = blobs->begin(); it != blobs->end(); it++){
		if(it->second.z > maxTouchZ){
			maxTouchZ = it->second.z;
		}	
	}
	float targetOpacity = ofMap(maxTouchZ, layerBarrierZ-layerBarrierWidth/2, layerBarrierZ+layerBarrierWidth/2, 1.0, 0.0, true);
	
	layer1Opacity += (targetOpacity - layer1Opacity) * .1; //dampen
	tweets.tweetLayerOpacity = layer1Opacity;
	
	tweets.update();
	caust.reset();
	
	
	
	
	vector<ofxWWTweetParticle>::iterator twit = tweets.tweets.begin();
	int i = 0;
	ofVec2f offset(tweets.dotShift, 0);

	if(tweets.searchTermManager.selectedSearchTermIndex!=-1) {
        searchTermLocation = tweets.searchTermManager.searchTerms[tweets.searchTermManager.selectedSearchTermIndex].pos;
		caust.addPoint(searchTermLocation-ofVec2f(5, 0),0);
		caust.addPoint(searchTermLocation+ofVec2f(5, 0),1);
	}
	while(twit!=tweets.tweets.end()) {

		if((*twit).dot_opacity>0) caust.addPoint((*twit).pos+offset, i++);
		twit++;
	}
	
	caust.update();
}

void ofxWWRenderer::render(){

	renderGradientOverlay();

	caust.renderToFbo();

	//blit to main render target
	renderTarget.begin();
	ofClear(0);
	ofEnableAlphaBlending();
	
	gradientOverlay.draw(0,0, targetWidth, targetHeight);
	
	

	tweets.renderSearchTerms();
	
	tweets.renderTweets();	
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);

	//if(causticsAlwaysOn) {
	//	ofSetHexColor(0xFFFFFF);
	//} else {

		
	//}

	

	searchTermHaloShader.begin();
	searchTermHaloShader.setUniformTexture("tex", caust.getFbo().getTextureReference(0), 0);
	searchTermHaloShader.setUniform2f("centre", searchTermLocation.x, searchTermLocation.y);
	searchTermHaloShader.setUniform1f("radius", causticHaloRadius);
	searchTermHaloShader.setUniform1f("amount", 1.0);
	if(causticsAlwaysOn) {
		searchTermHaloShader.setUniform1f("__alpha", 1.0);		
	} else {
		searchTermHaloShader.setUniform1f("__alpha", 1.0 - tweets.tweetLayerOpacity);
	}

	//printf("Alpha %f\n", 1.0-tweets.tweetLayerOpacity);

    glColor4f(1,1,1,0);//1.f - tweets.tweetLayerOpacity);

	caust.getFbo().draw(0, 0);
	searchTermHaloShader.end();
	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		
	callToAction.draw();
	
		
	float maxTouchRadius = targetHeight*tweets.touchSizeScale;	
	map<int,KinectTouch>::iterator it;
	if(drawTouchDebug){ 
		ofPushStyle();
		ofNoFill();
		for(it = blobs->begin(); it != blobs->end(); it++){
			ofVec2f touchCenter = ofVec2f( it->second.x*targetWidth, it->second.y*targetHeight );

			ofSetColor(255, 255, 255);
			ofCircle(touchCenter, it->second.size*maxTouchRadius);			
			ofSetColor(0, 255, 0);
			ofCircle(touchCenter, it->second.size*(maxTouchRadius - tweets.touchInfluenceFalloff/2));
			ofSetColor(255, 255, 0);
			ofCircle(touchCenter, it->second.size*(maxTouchRadius + tweets.touchInfluenceFalloff/2));
			
			ofPushMatrix();
			ofTranslate(touchCenter.x, touchCenter.y);
			ofScale(10, 10);
			ofDrawBitmapString("Z:"+ofToString(it->second.z,4), ofVec2f(0,0));
			ofPopMatrix();
		}
		ofPopStyle();
	}
	
	ofPushStyle();
    {
		ofColor surfaceHalo = ofColor::fromHex(Colours::get(HALO_SURFACE));
		ofColor bottomHalo = ofColor::fromHex(Colours::get(HALO_SEARCH));
		
		float tweenedSmootherStep = smootherStep(layer1Opacity, 0.f, 1.f);
		
		surfaceHalo.lerp(bottomHalo, tweenedSmootherStep);
		
		//ofEnableBlendMode(OF_BLENDMODE_ADD);
		ofSetColor(surfaceHalo.r, surfaceHalo.g, surfaceHalo.b, 100);
		for(it = blobs->begin(); it != blobs->end(); it++){
			ofVec2f touchCenter = ofVec2f( it->second.x*targetWidth, it->second.y*targetHeight );
			float radius = it->second.size*maxTouchRadius;
			halo.draw(ofRectangle(touchCenter.x-radius,touchCenter.y-radius, radius*2,radius*2));
		}
		//ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	}
	ofPopStyle();
	
	renderTarget.end();	
}

//roxlu: testing screenshots
void ofxWWRenderer::keyPressed(ofKeyEventArgs& args) {
//	if(args.key == '1') {
//		test_screenshot = true;
//	}
}

void ofxWWRenderer::renderLayer1(){
	layer1Target.begin();
	ofClear(0,0,0,0);
	tweets.renderTweets();	
	layer1Target.end();
}

void ofxWWRenderer::renderLayer2(){
	layer2Target.begin();
	ofClear(0,0,0,0);
	
	tweets.renderSearchTerms();
	
	layer2Target.end();
}

void ofxWWRenderer::renderGradientOverlay(){
	ofPushStyle();
	gradientOverlay.begin();
	ofClear(0);
	ofEnableAlphaBlending();
	
	ofColor surface = ofColor::fromHex(Colours::get(SURFACE_BG));
    ofColor bottom = ofColor::fromHex(Colours::get(SEARCH_BG));
    
    surface.lerp(bottom, 1-layer1Opacity);
    
    ofSetColor(surface);
    
    ofRect(0, 0, background.getWidth(), background.getHeight());
	ofPopStyle();
	
	gradientOverlay.end();
}

void ofxWWRenderer::renderBackground(){
	ofPushStyle();
	background.begin();
	ofClear(0);
	ofEnableAlphaBlending();
    
    ofColor surface = ofColor::fromHex(Colours::get(SURFACE_BG));
    ofColor bottom = ofColor::fromHex(Colours::get(SEARCH_BG));
    
    surface.lerp(bottom, 1-layer1Opacity);
    
    ofSetColor(surface);
    
    ofRect(0, 0, background.getWidth(), background.getHeight());
	
	background.end();
	ofPopStyle();    
}

ofFbo& ofxWWRenderer::getFbo(){
	return renderTarget;
}

void ofxWWRenderer::touchDown(const KinectTouch &touch) {
	tweets.touchDown();
}

void ofxWWRenderer::touchMoved(const KinectTouch &touch) {

	fluid.applyForce(ofVec2f(touch.x, touch.y), ofVec2f(touch.vel.x, touch.vel.y));
	callToAction.justInteracted();
}

void ofxWWRenderer::touchUp(const KinectTouch &touch) {
//	tweets.resetTouches();
	tweets.touchUp();
}

ofxWWTweetParticleManager& ofxWWRenderer::getTweetManager() {
	return tweets;
}

ofxWWSearchTermManager& ofxWWRenderer::getSearchTermManager() {
	return tweets.getSearchTermManager();
}


void ofxWWRenderer::stopFluidThread(){
	fluid.stopThread(true);
}

ofVec2f ofxWWRenderer::randomPointInCircle(ofVec2f position, float radius){
	float randomRadius = radius * ofRandomuf();
	float randomAngle = ofRandom(360.*DEG_TO_RAD);
	
	float x = randomRadius * cos(randomAngle);
	float y = randomRadius * sin(randomAngle);
	
	return position + ofVec2f(x,y);
}

