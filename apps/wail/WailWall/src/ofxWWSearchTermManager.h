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
	
	
	ofxWWSearchTermManager();
	void setup(TwitterApp *twitter, ofxWWTweetParticleManager *parent);
	void update();
	
	void render();
	void addSearchTerm(const string& user, const string& term);
	void deselectAllSearchTerms();
	
	
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

