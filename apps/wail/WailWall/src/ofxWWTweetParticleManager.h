/*
 *  ofxWWTweetManager.h
 *  WailWall
 *
 *  Created by James George on 1/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once

#include "ofMain.h"
#include "ofxWWTweetParticle.h"
#include "ofxWWSearchTerm.h"
#include "TwitterApp.h"
#include "ofxFTGLFont.h"
#include "ofxMPMFluid.h"

class ofxWWTweetParticleManager : public roxlu::twitter::IEventListener {
  public:
	ofxWWTweetParticleManager();
	void setup();
	void setupGui();
	
	void update();
	void renderTweets();
	void renderSearchTerms();
	
	void onStatusUpdate(const rtt::Tweet& tweet);
	void onStatusDestroy(const rtt::StatusDestroy& destroy);
	void onStreamEvent(const rtt::StreamEvent& event);
	void onNewSearchTerm(TwitterAppEvent& event);

	//ofxFTGLFont sharedFont;
	ofxMPMFluid* fluidRef;
	ofTrueTypeFont sharedFont;
	ofTrueTypeFont sharedLargeFont;
	
	int maxTweets;
	
	float simulationWidth;
	float simulationHeight;

	float wallRepulsionDistance;
	float wallRepulsionAtten;
	float tweetRepulsionDistance;
	float tweetRepulsionAtten;
	float fluidForceScale;
	float yForceBias;
	
	float twoLineScaleup;
	float userNameYOffset;
	float userNameXPad;
	float twoLineSquish; //todo
	
	
	float fontSize;
	float wordWrapLength;
	
	bool clearTweets;
  protected:
	TwitterApp twitter;
	vector<ofxWWTweetParticle> tweets;
	vector<ofxWWSearchTerm*> searchTerms;
	
};
