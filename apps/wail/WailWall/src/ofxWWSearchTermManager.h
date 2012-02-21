#pragma once

#include "ofxWWSearchTerm.h"
#include "TwitterApp.h"

typedef void (*takeScreenshotCallback)(const string& username, void* userdata);

class ofxWWTweetParticleManager;

class ofxWWSearchTermManager {
public:
	
	
	ofxWWSearchTermManager();
	void setup(TwitterApp *twitter, ofxWWTweetParticleManager *parent);
	void update();
	
	void render();
	void addSearchTerm(const string& user, const string& term, bool isUsed = false);
	void deselectAllSearchTerms();
	void doSearchTermSelectionTest();
	
	// screenshots
	void setScreenshotCallback(takeScreenshotCallback func, void* user);
	takeScreenshotCallback  screenshot_callback;
	void* screenshot_userdata;
	float should_take_picture_on;
	ofxWWSearchTerm screenshot_searchterm;
	
	queue<ofxWWSearchTerm> incomingSearchTerms;
	vector<ofxWWSearchTerm> searchTerms;
	
	int selectedSearchTermIndex;
	float lastSearchTermTime;
	float tweetSearchMinWaitTime;
	int searchTermFontSize;
	
	
	
	int maxSearchTerms;
	float searchTermMinDistance;
	float searchTermMinHoldTime;
	float searchMinOpacity;
	float searchTermRepulsionDistance;
	float searchTermRepulsionAttenuation;
	float searchTermHandAttractionFactor;
	float searchTermFadeOutTime;
	
	bool drawSearchDebug;
	
	
	ofxWWTweetParticleManager *parent;
	
	void handleTouchSearch();
	void handleTweetSearch();
	void doTouchInteraction();
private:
	TwitterApp *twitter;
};

