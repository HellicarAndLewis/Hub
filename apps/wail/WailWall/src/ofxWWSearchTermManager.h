#pragma once

#include "ofxWWSearchTerm.h"
#include "TwitterApp.h"

typedef void (*takeScreenshotCallback)(const string& username, void* userdata);

class ofxWWTweetParticleManager;

class ofxWWSearchTermManager {
public:
	
	ofxWWTweetParticleManager *parent;
	
	// initialization
	ofxWWSearchTermManager();
	void setup(TwitterApp *twitter, ofxWWTweetParticleManager *parent);
	
	
	// update data and touches
	void update();

	// render to FBO
	void render(); 
		  
	
	// call this to add a search term
	void addSearchTerm(const string& user, const string& term, bool isUsed);
	
	// call this to deslect everything

	void deselectAllSearchTerms();
	

	// screenshots
	void setScreenshotCallback(takeScreenshotCallback func, void* user);
	takeScreenshotCallback  screenshot_callback;
	void* screenshot_userdata;
	float should_take_picture_on;
	ofxWWSearchTerm screenshot_searchterm;
	

	vector<ofxWWSearchTerm> searchTerms;
	
	int selectedSearchTermIndex;
	float lastSearchTermTime;
	float tweetSearchMinWaitTime;
	int searchTermFontSize;
	
	int maxSearchTerms;



	float repulsionDistance;
	float repulsionAttenuation;
	float fadeOutTime;
	
	bool drawSearchDebug;
	
	
	
private:
	void handleTouchSearch();
	void handleTweetSearch();
	void doTouchInteraction();
	void doSearchTermSelectionTest();
	

	TwitterApp *twitter;
	queue<ofxWWSearchTerm> incomingSearchTerms;
};

