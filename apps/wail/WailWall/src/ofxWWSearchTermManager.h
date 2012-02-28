#pragma once

#include "ofxWWSearchTerm.h"
#include "TwitterApp.h"
#include "SearchLayerListener.h"
#include "Timer.h"


typedef void (*takeScreenshotCallback)(const string& username, void* userdata);


class ofxWWTweetParticleManager;

class ofxWWSearchTermManager: public SearchLayerListener {
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
	

	// this sets the selected search term and fires events.
	void setSelectedSearchTerm(ofxWWSearchTerm& searchTerm);
	bool getSelectedSearchTerm(ofxWWSearchTerm& result);
	bool getSearchTermForWhichWeNeedToTakeScreenshot(ofxWWSearchTerm& term);
	bool setTookScreenshotForSearchTerm(ofxWWSearchTerm& term);
	void selectSearchTerm(const string& term);
	void deselectSearchTerm(const string& term);
		
	// search term manager only needs
	// to know when these events happen
	// for (de)selection purposes.
	void touchUp();
	void touchDown();

	
	void addSearchLayerListener(SearchLayerListener *listener);
	vector<SearchLayerListener*> listeners;

	

	// screenshots
	void setScreenshotCallback(takeScreenshotCallback func, void* user);
	takeScreenshotCallback  screenshot_callback;
	void* screenshot_userdata;
	float should_take_picture_on;
	ofxWWSearchTerm screenshot_searchterm;

	
	// the brightness of search terms when not on the bottom layer.
	float searchTermMinAlpha;
	
	// the min brightness of an actual selected search term.
	float selectedTermMinAlpha;

	vector<ofxWWSearchTerm> searchTerms;
	
	int selectedSearchTermIndex;
	float lastSearchTermTime;
	float tweetSearchMinWaitTime;
	int searchTermFontSize;
	
	float searchTermSelectionRadiusPercent;
	int maxSearchTerms;



	float repulsionDistance;
	float repulsionAttenuation;
	float fadeOutTime;
	
	bool drawSearchDebug;
	
	
	float deselectionDelay;
	

	void onSearchTermSelected(const SearchTermSelectionInfo& term);
	void onAllSearchTermsDeselected();
	
	
	
	// how much the search terms move
	float movement;
	
	
private:
	Timer handRemovedTimer;
	void handleTweetSearch();
	void doTouchInteraction();
	void doSearchTermSelectionTest();
	
	// this is the last selection term that was sent as
	// an event, so we don't send duplicates
	string lastSearchTermSelectionSentAsEvent;
	
	TwitterApp *twitter;
	queue<ofxWWSearchTerm> incomingSearchTerms;
	
};

