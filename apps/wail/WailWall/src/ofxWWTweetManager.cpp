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
	if(!twitter.initDB()){
		printf("Error: cannot initialize twitter db.\n");
	}
	
	//twitter.addDefaultListener();
	twitter.addCustomListener(*this);
	
	// What do you want to track?
	twitter.track("ItsFunnyHow");
	twitter.track("NeverTrustAGuyWho");
	twitter.track("TolimaDay");	
	twitter.track("love");
	
	if(!twitter.connect()) {
		printf("Error: cannot connect to twitter stream.\n");
	}
	
}

void ofxWWTweetManager::update(){
	twitter.update();
}

void ofxWWTweetManager::renderTweets(){
	
}

void ofxWWTweetManager::renderSearchTerms(){

}


void ofxWWTweetManager::onStatusUpdate(const rtt::Tweet& tweet){
	
}

void ofxWWTweetManager::onStatusDestroy(const rtt::StatusDestroy& destroy){
}

void ofxWWTweetManager::onStreamEvent(const rtt::StreamEvent& event){
}
