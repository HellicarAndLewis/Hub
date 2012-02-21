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

#include "TweetProvider.h"
#include "TweetProviderStream.h"
#include "TweetProviderDB.h"
#include "TweetProviderListener.h"
//#include "TwitterMentionsListener.h"


typedef void (*takeScreenshotCallback)(const string& username, void* userdata);

class ofxWWRenderer;

class ofxWWTweetParticleManager : public TweetProviderListener {
// : public roxlu::twitter::IEventListener {
  public:
  
	ofxWWTweetParticleManager();
	
	void setup(ofxWWRenderer* ren);
	
	void setupGui();
	
	void update();

	void renderTweets();
	
	void renderSearchTerms();

	void renderConnections();
	
	void onNewTweet(const rtt::Tweet& tweet);
	/*
	void onStatusUpdate(const rtt::Tweet& tweet);
	void onStatusDestroy(const rtt::StatusDestroy& destroy);
	void onStreamEvent(const rtt::StreamEvent& event);
 */
	void onNewSearchTerm(TwitterAppEvent& event);
	void setupColors();
	
	TwitterApp& getTwitterApp();
	
	//ofxFTGLFont sharedFont;
	ofxMPMFluid* fluidRef;
	map<int,KinectTouch>* blobsRef;
	


	
	
	
	
	// ofXFTGFont
	
	#ifdef USE_FTGL
		ofxFTGLFont sharedTweetFont;
		ofxFTGLFont sharedUserFont;
		ofxFTGLFont sharedSearchFont;
	#else	
		ofTrueTypeFont sharedTweetFont;
		ofTrueTypeFont sharedUserFont;
		ofTrueTypeFont sharedSearchFont;
	#endif
	
	
	
	// this is how much inactive time triggers
	// a call to action.
	float callToActionTime;
	
	
	
	bool enableCaustics;
	float causticFadeSpeed;
	
	bool canSelectSearchTerms;
	float tweetLayerOpacity;
	float touchSizeScale;
	float touchInfluenceFalloff;
	
	float simulationWidth;
	float simulationHeight;
	
	bool drawTweetDebug;
	int maxTweets;
	float startFadeTime;
	float fadeDuration;
	
	float wallRepulsionDistance;
	float wallRepulsionAtten;
	float tweetRepulsionDistance;
	float tweetRepulsionAtten;
	float fluidForceScale;
		
	bool tweetsFlowLeftRight; //otherwise up/down
	float tweetFlowSpeed;
	float tweetFlowVariance;
	float tweetRotateAmp;
	float tweetRotateDamp;
	float tweetChaosSpeed;
	float tweetFlowDamp;
	float tweetFlowAmp;
	
	//tweet rendering
	int userFontSize;
	int tweetFontSize;
	int searchTermFontSize;
	float dotSize;
	float dotShift;
	float wordWrapLength;
	float userNameYOffset;
	float tweetYOffset;
	float tweetLineSpace;
	
	bool clearTweets;
	
	int maxSearchTerms;
	float searchTermMinDistance;
	float searchTermMinHoldTime;
	float searchMinOpacity;
	float searchTermRepulsionDistance;
	float searchTermRepulsionAttenuation;
	float searchTermHandAttractionFactor;
	float searchTermFadeOutTime;
	
	bool drawSearchDebug;
	
	float tweetSearchMinWaitTime;
	float tweetSearchDuration;

	bool isDoingSearch;
	bool shouldTriggerScreenshot;
	
	vector<ofxWWTweetParticle> tweets;
	vector<ofColor> causticColors;

	//particle images
	ofImage burstOne;
	ofImage burstTwo;

	//particle colors
	ofColor atSignColor;
	ofColor layerOneFontColor;
	ofColor layerTwoFontColor;
	
	// search state vars
	int shouldChangeSearchTermOn;	// when do show the next search term.
	int changeSearchTermDelay; 		// after how many seconds do go to the next search term in queue.
	int currentSearchTermIndex;
	
	void keyPressed(ofKeyEventArgs& args); //JG just used for simulating searches

	// Screenshot
	void addCurrentRenderToScreenshotQueue();
	void setScreenshotCallback(takeScreenshotCallback func, void* user);
	takeScreenshotCallback  screenshot_callback;
	void* screenshot_userdata;
	
	void touchUp();
	
  protected:
	TwitterApp twitter;
	
	vector<string> fakeSearchTerms;	
	queue<ofxWWSearchTerm> incomingSearchTerms;
	vector<ofxWWSearchTerm> searchTerms;
	
//	bool searchTermSelected;
	int selectedSearchTermIndex;
	
	void checkFonts();
	
	void handleSearch();
	void handleTouchSearch();
	void handleTweetSearch();	
	void finishSearch();
	
	float weightBetweenPoints(ofVec2f touch, float normalizedSize, ofVec2f tweet);
	void updateTweets();
	void updateSearchTerms();
	
	void attemptConnection(ofVec2f pos1, float weight1, ofVec2f pos2, float weight2, float layerOpacity);
	void setRandomCausticColor(float layerOpacity);
	
	int numSearchTermTweets; //for debugging
	void addSearchTerm(const string& user, const string& term);
	ofxWWTweetParticle createParticleForTweet(const rtt::Tweet& tweet);
	
	ofxWWRenderer* renderer;
	
	// providers for tweets
	void setCurrentProvider(TweetProvider* prov);
	TweetProvider* current_provider;
	TweetProviderStream* stream_provider;
	TweetProviderDB* db_provider;
	//virtual void onNewSearchTermFromPollingAPI(const rtt::Tweet& tweet, const string& term);
	
	float lastSearchTermTime;
	float should_take_picture_on;
	
};
