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
#include "KinectTouchListener.h"

class ofxWWTweetParticleManager : public roxlu::twitter::IEventListener {
  public:
	ofxWWTweetParticleManager();
	void setup();
	void setupGui();
	
	void update();
	
	void renderTweets();
	void renderTweetNodes();
	void renderSearchTerms();

	void renderCaustics();
	
	void onStatusUpdate(const rtt::Tweet& tweet);
	void onStatusDestroy(const rtt::StatusDestroy& destroy);
	void onStreamEvent(const rtt::StreamEvent& event);
	void onNewSearchTerm(TwitterAppEvent& event);

	TwitterApp& getTwitterApp();
	
	//ofxFTGLFont sharedFont;
	ofxMPMFluid* fluidRef;
	map<int,KinectTouch>* blobsRef;
	
	ofTrueTypeFont sharedFont;
	ofTrueTypeFont sharedLargeFont;
	ofTrueTypeFont sharedSearchFont;
	
	bool enableCaustics;
	bool canSelectSearchTerms;
	float tweetLayerOpacity;
	float touchSizeScale;
	float touchInfluenceFalloff;
	
	float simulationWidth;
	float simulationHeight;
	
	int maxTweets;
	float startFadeTime;
	float fadeDuration;
	
	float wallRepulsionDistance;
	float wallRepulsionAtten;
	float tweetRepulsionDistance;
	float tweetRepulsionAtten;
	float fluidForceScale;
	float yForceBias;

	float twoLineScaleup;
	float userNameYOffset;
	float tweetYOffset;
	
	float dotSize;
	float fontSize;
	float searchTermFontSize;
	float wordWrapLength;
	
	bool clearTweets;
	
	bool generateFakeSearchTerms;
	float searchTermMinDistance;
	float searchTermMinHoldTime;

	vector<ofxWWTweetParticle> tweets;
	vector<ofColor> causticColors;

	//particle images
	ofImage burstOne;
	ofImage burstTwo;

  protected:
	TwitterApp twitter;
	vector<ofxWWSearchTerm> searchTerms;
	
	
	bool searchTermSelected;
	int selectedSearchTerm;
	
	void handleTouchSearch();
	void handleTweetSearch();
	
	float weightBetweenPoints(ofVec2f touch, float normalizedSize, ofVec2f tweet);
	void updateTweets();
	
	void attemptCausticConnection(ofVec2f pos1, float weight1, ofVec2f pos2, float weight2);
	void setRandomCausticColor();
	
	
	ofxWWTweetParticle createParticleForTweet(const rtt::Tweet& tweet);
	
};
