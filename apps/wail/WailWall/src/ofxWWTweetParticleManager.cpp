/*
 *  ofxWWTweetManager.cpp
 *  WailWall
 *
 *  Created by James George on 1/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxWWTweetParticleManager.h"
#include "ofxSimpleGuiToo.h"

ofxWWTweetParticleManager::ofxWWTweetParticleManager(){
	maxTweets = 100;
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

void ofxWWTweetParticleManager::setupGui(){
	gui.addPage("Tweet Particles");
	
	gui.addSlider("Tweet Font Size", fontSize, 5, 24);
	gui.addSlider("Word Wrap Length", wordWrapLength, 100, 300);
	gui.addSlider("Max Tweets", maxTweets, 5, 100);

	gui.addSlider("Two Line Scale", twoLineScaleup, 1.0, 2.0);
	gui.addSlider("User Y Shift", userNameYOffset, -10, 20);
	gui.addSlider("User X Padding", userNameXPad, -2, 10);
	gui.addSlider("Two Line Squish", twoLineSquish, .5, 1.0);
	gui.addSlider("Wall Repulsion Dist", wallRepulsionDistance, 0, 300);
	gui.addSlider("Wall Repulsion Atten", wallRepulsionAtten, 0, .5);
	gui.addSlider("Tweet Repulsion Dist", tweetRepulsionDistance, 0, 300);
	gui.addSlider("Tweet Repulsion Atten", tweetRepulsionAtten, 0, .5);
	
	gui.addToggle("Clear Tweets", clearTweets);
}


void ofxWWTweetParticleManager::update(){
	
	if(clearTweets){
		tweets.clear();
		clearTweets = false;
	}
	
	if(!sharedFont.isLoaded() || fontSize != sharedFont.getSize() || int(fontSize*twoLineScaleup) != sharedLargeFont.getSize()){
		if(!sharedFont.loadFont("fonts/Tahoma.ttf", fontSize, true, true, false) ||
		   !sharedLargeFont.loadFont("fonts/Tahoma.ttf", fontSize*twoLineScaleup, true, true, false)){
			ofLogError("ofxWWTweetParticleManager::setup() -- couldn't load font!");
		}
	}
	
	twitter.update();
	
	
	//purge dead tweets
	for(int i = tweets.size()-1; i >= 0; i--){
		if(tweets[i].dead){
			tweets.erase(tweets.begin()+i);
		}
	}
	
	if(tweets.size() > maxTweets){
		tweets.resize(maxTweets);
	}
	
	//apply wall forces
	for(int i = 0; i < tweets.size(); i++){
		
		if (tweets[i].pos.x < wallRepulsionDistance) {
			tweets[i].force.x += (wallRepulsionDistance - tweets[i].pos.x) * wallRepulsionAtten;
		}
		if ((tweets[i].pos.x + tweets[i].totalWidth) > (simulationWidth-wallRepulsionDistance)) {
			tweets[i].force.x += ( (simulationWidth-wallRepulsionDistance) - (tweets[i].pos.x + tweets[i].totalWidth) ) * wallRepulsionAtten;
		}

		if (tweets[i].pos.y < wallRepulsionDistance) {
			tweets[i].force.y += (wallRepulsionDistance - tweets[i].pos.y) * wallRepulsionAtten;
		}
		if ((tweets[i].pos.y + tweets[i].totalHeight) > (simulationHeight-wallRepulsionDistance)) {
			tweets[i].force.y += ( (simulationHeight-wallRepulsionDistance)  - (tweets[i].pos.y + tweets[i].totalHeight)) * wallRepulsionAtten;
		}
		
	}
	
	//apply mutual repulsion
	for(int i = 0; i < tweets.size(); i++){
		for(int j = 0; j < tweets.size(); j++){
			if(i != j){
				ofVec2f awayFromOther = (tweets[i].pos - tweets[j].pos);
				float distance = awayFromOther.length();
				awayFromOther.normalize();
				if(distance < tweetRepulsionDistance){
					tweets[i].force += (awayFromOther * (tweetRepulsionDistance - distance) * tweetRepulsionAtten);
				}
			}
		}
	}
	
	
	for(int i = 0; i < tweets.size(); i++){
		tweets[i].update();
	}
}

void ofxWWTweetParticleManager::onNewSearchTerm(TwitterAppEvent& event) {
	printf("Yay I got a new search term: %s\n", event.search_term.c_str());
}

void ofxWWTweetParticleManager::renderTweets(){
	for(int i = 0; i < tweets.size(); i++){
		tweets[i].draw();
	}
}

void ofxWWTweetParticleManager::renderSearchTerms(){
	
}


void ofxWWTweetParticleManager::onStatusUpdate(const rtt::Tweet& tweet){
	ofxWWTweetParticle tweetParticle;
	tweetParticle.manager = this;
	tweetParticle.pos = ofVec2f(ofRandom(simulationWidth-wordWrapLength), ofRandom(simulationHeight));
	tweetParticle.setTweet(tweet);
	tweets.push_back( tweetParticle );
	
	cout << "added tweet " << tweets.size() << " text is " << tweet.getText() << endl;
}

void ofxWWTweetParticleManager::onStatusDestroy(const rtt::StatusDestroy& destroy){
}

void ofxWWTweetParticleManager::onStreamEvent(const rtt::StreamEvent& event){
}
