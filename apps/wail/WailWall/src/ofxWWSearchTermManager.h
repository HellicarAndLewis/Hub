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
	void addSearchTerm(const string& user, const string& term);
	
	// call this to deslect everything
	void deselectAllSearchTerms();
	
	
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

