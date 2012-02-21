//
//  ofxWWSearchTermManager.h
//  WailWall
//
//  Created by Joel Lewis on 20/02/2012.
//  Copyright (c) 2012 Hellicar & Lewis. All rights reserved.
//

#pragma once

#include "ofxWWSearchTerm.h"
#include "TwitterApp.h"
#include "SearchLayerListener.h"
#include "Timer.h"

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
	void addSearchTerm(const string& user, const string& term);
	
	// this sets the selected search term and fires events.
	void setSelectedSearchTerm(ofxWWSearchTerm &searchTerm);

		
	// search term manager only needs
	// to know when these events happen
	// for (de)selection purposes.
	void touchUp();
	void touchDown();
	

	
	void addListener(SearchLayerListener *listener);
	vector<SearchLayerListener*> listeners;
	
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
	
	
	float deselectionDelay;
	
	// for testing
	void onSearchTermSelected(const SearchTermSelectionInfo& term);
	void onAllSearchTermsDeselected();
	
	
	
private:
	Timer handRemovedTimer;
	void handleTouchSearch();
	void handleTweetSearch();
	void doTouchInteraction();
	void doSearchTermSelectionTest();
	
	// this is the last selection term that was sent as
	// an event, so we don't send duplicates
	string lastSearchTermSelectionSentAsEvent;
	
	TwitterApp *twitter;
	queue<ofxWWSearchTerm> incomingSearchTerms;
};

