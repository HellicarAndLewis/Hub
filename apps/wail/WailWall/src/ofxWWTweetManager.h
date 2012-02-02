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
#include "ofxWWTweet.h"
#include "ofxWWSearchTerm.h"
#include "TwitterApp.h"

class ofxWWTweetManager : public roxlu::twitter::IEventListener {
  public:
	ofxWWTweetManager();
	void setup();
	void update();
	void renderTweets();
	void renderSearchTerms();
	
	void onStatusUpdate(const rtt::Tweet& tweet);
	void onStatusDestroy(const rtt::StatusDestroy& destroy);
	void onStreamEvent(const rtt::StreamEvent& event);
	void onNewSearchTerm(const string& term);
	
  protected:
	vector<ofxWWTweet*> tweets;
	vector<ofxWWSearchTerm*> searchTerms;
	
	TwitterApp twitter;
};
