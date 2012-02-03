/*
 *  ofxWWTweetManager.cpp
 *  WailWall
 *
 *  Created by James George on 1/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxWWTweetParticleManager.h"

ofxWWTweetParticleManager::ofxWWTweetParticleManager(){
	maxTweets = 5000;
}

void ofxWWTweetParticleManager::setup(){
	
	if(!twitter.initDB()){
		printf("Error: cannot initialize twitter db.\n");
	}
	
	//twitter.addDefaultListener();
	twitter.addCustomListener(*this);
	
	// What do you want to track?
	twitter.track("love");
	
	if(!twitter.connect()) {
		printf("Error: cannot connect to twitter stream.\n");
	}
	
	twitter.addListener(this, &ofxWWTweetParticleManager::onNewSearchTerm);
}

void ofxWWTweetParticleManager::update(){
	twitter.update();
}

void ofxWWTweetParticleManager::onNewSearchTerm(TwitterAppEvent& event) {
	printf("Yay I got a new search term: %s\n", event.search_term.c_str());
}

void ofxWWTweetParticleManager::renderTweets(){
	
}

void ofxWWTweetParticleManager::renderSearchTerms(){

}


void ofxWWTweetParticleManager::onStatusUpdate(const rtt::Tweet& tweet){
	ofxWWTweetParticle tweetParticle;
	tweetParticle.tweet = tweet;
	cout << "TWEET: " << tweet.getText() << endl;
}

void ofxWWTweetParticleManager::onStatusDestroy(const rtt::StatusDestroy& destroy){
}

void ofxWWTweetParticleManager::onStreamEvent(const rtt::StreamEvent& event){
}
