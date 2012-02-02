/*
 *  ofxWWTweetManager.cpp
 *  WailWall
 *
 *  Created by James George on 1/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxWWTweetManager.h"

ofxWWTweetManager::ofxWWTweetManager(){

}

void ofxWWTweetManager::setup(){
	twitter.addTwitterListener( *this );
}

void ofxWWTweetManager::update(){
	twitter.update();
}

void ofxWWTweetManager::renderTweets(){
	
}

void ofxWWTweetManager::renderSearchTerms(){

}


void ofxWWTweetManager::onStatusUpdate(const rtt::Tweet& tweet){
	cout << "TWEET!" << endl;
}

void ofxWWTweetManager::onStatusDestroy(const rtt::StatusDestroy& destroy){
}

void ofxWWTweetManager::onStreamEvent(const rtt::StreamEvent& event){
}
