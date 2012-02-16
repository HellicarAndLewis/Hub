/*
 *  ofxWWTweetManager.h
 *  WailWall
 *
 *  Created by James George on 1/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */
#pragma once
#define USE_FTGL

#include "ofMain.h"
#include "ofxWWTweetParticle.h"
#include "ofxWWSearchTerm.h"
#include "TwitterApp.h"
#include "TwitterSearchTerm.h"
#include "ofxFTGLFont.h"
#include "ofxMPMFluid.h"
#include "KinectTouchListener.h"

class ofxWWRenderer;

class ofxWWTweetParticleManager : public roxlu::twitter::IEventListener {
  public:
	ofxWWTweetParticleManager();
	void setup(ofxWWRenderer* ren);
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
	
	// ofXFTGFont


	#ifdef USE_FTGL
		ofxFTGLFont sharedFont;
		ofxFTGLFont sharedLargeFont;
		ofxFTGLFont sharedSearchFont;
	#else	
		ofTrueTypeFont sharedFont;
		ofTrueTypeFont sharedLargeFont;
		ofTrueTypeFont sharedSearchFont;
	#endif
	
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
	
	bool tweetsFlowLeftRight; //otherwise up/down
	float tweetFlowSpeed;
	float flowChaosScale;
	
	float dotSize;
	float fontSize;
	float searchTermFontSize;
	float wordWrapLength;
	
	bool clearTweets;
	
	float searchTermMinDistance;
	float searchTermMinHoldTime;

	vector<ofxWWTweetParticle> tweets;
	vector<ofColor> causticColors;

	//particle images
	ofImage burstOne;
	ofImage burstTwo;

	// search state vars
	int shouldChangeSearchTermOn;	// when do show the next search term.
	int changeSearchTermDelay; 		// after how many seconds do go to the next search term in queue.
	int currentSearchTermIndex;
	
	void keyPressed(ofKeyEventArgs& args); //JG just used for simulating searches
	
  protected:
	TwitterApp twitter;
	
	vector<string> fakeSearchTerms;	
	queue<ofxWWSearchTerm> incomingSearchTerms;
	vector<ofxWWSearchTerm> searchTerms;
	
//	bool searchTermSelected;
	int selectedSearchTermIndex;
	
	void handleSearch();
	void handleTouchSearch();
	void handleTweetSearch();	
	void searchForTerm(ofxWWSearchTerm& term);	
	void finishSearch();
	
	float weightBetweenPoints(ofVec2f touch, float normalizedSize, ofVec2f tweet);
	void updateTweets();
	
	void attemptCausticConnection(ofVec2f pos1, float weight1, ofVec2f pos2, float weight2, float layerOpacity);
	void setRandomCausticColor(float layerOpacity);
	
	void addCurrentRenderToScreenshotQueue();
	void addSearchTerm(const string& user, const string& term);
	ofxWWTweetParticle createParticleForTweet(const rtt::Tweet& tweet);
	
	float tweetSearchMinWaitTime;
	float tweetSearchDuration;
	float tweetSearchStartTime;
	float tweetSearchEndedTime;
	bool isDoingSearch;
	bool shouldTriggerScreenshot;
	
	ofxWWRenderer* renderer;
	GLuint pbo;

};
