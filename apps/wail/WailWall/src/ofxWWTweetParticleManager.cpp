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
	//twitter.track("love");
	twitter.track("usa");
	
	if(!twitter.connect()) {
		printf("Error: cannot connect to twitter stream.\n");
	}
	
	twitter.addListener(this, &ofxWWTweetParticleManager::onNewSearchTerm);
	
}

void ofxWWTweetParticleManager::setupGui(){
	
	gui.addPage("Tweet Lifecycle");
	gui.addSlider("Tweet Font Size", fontSize, 5, 24);
	gui.addSlider("Word Wrap Length", wordWrapLength, 100, 300);
	gui.addSlider("Max Tweets", maxTweets, 5, 100);
	gui.addSlider("Start Fade Time", startFadeTime, 2, 10);
	gui.addSlider("Fade Duration", fadeDuration, 2, 10);
	gui.addToggle("Clear Tweets", clearTweets);

	
	gui.addPage("Tweet Appearance");
	gui.addSlider("Two Line Scale", twoLineScaleup, 1.0, 2.0);
	gui.addSlider("User Y Shift", userNameYOffset, -10, 20);
	gui.addSlider("User X Padding", userNameXPad, -2, 10);
	gui.addSlider("Two Line Squish", twoLineSquish, .5, 1.0);
	gui.addSlider("Wall Repulsion Dist", wallRepulsionDistance, 0, 300);
	gui.addSlider("Wall Repulsion Atten", wallRepulsionAtten, 0, .5);
	gui.addSlider("Tweet Repulsion Dist", tweetRepulsionDistance, 0, 300);
	gui.addSlider("Tweet Repulsion Atten", tweetRepulsionAtten, 0, .5);
	gui.addSlider("Y Force Bias", yForceBias, 1., 10.);
	gui.addSlider("Fluid Force Scale", fluidForceScale, 1., 100.);
	
	gui.addPage("Search Terms");
//	gui.addToggle("Gen Fake Terms", generateFakeSearchTerms);
	gui.addSlider("Search Min Dist", searchTermMinDistance, 50, 500);
	gui.addSlider("Search Min Hold T", searchTermMinHoldTime, .5, 3.0);

	generateFakeSearchTerms = true;
}


void ofxWWTweetParticleManager::update(){

	if(clearTweets){
		tweets.clear();
		clearTweets = false;
	}
	
	//////////////////JUST FOR TESTING SEARCH TERMS BEFORE ITS IMPLEMENTED FULLY
	if(generateFakeSearchTerms){
		vector<string> fakeTerms;
		fakeTerms.push_back("DESIGN");
		fakeTerms.push_back("ECONOMICS");
		fakeTerms.push_back("INTERNET");
		fakeTerms.push_back("GENETICS");
		fakeTerms.push_back("EDUCATION");
		fakeTerms.push_back("SUSTAINABILITY");
		fakeTerms.push_back("OPENFRAMEWORKS");
		fakeTerms.push_back("GREEN");
		fakeTerms.push_back("INNOVATION");
		fakeTerms.push_back("GOOGLE");
		fakeTerms.push_back("INTELLIGENCE");
		
		twitter.populateFakeSearchTerms(fakeTerms);
		generateFakeSearchTerms = false;
	}
	//////////////////END TESTING
	
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
	
	///ANIMATE TWEETS
	//apply wall forces
	for(int i = 0; i < tweets.size(); i++){
		
		if (tweets[i].pos.x < wallRepulsionDistance) {
			tweets[i].force.x += (wallRepulsionDistance - tweets[i].pos.x) * wallRepulsionAtten * tweets[i].deathAttenuation;
		}
		if ((tweets[i].pos.x + tweets[i].totalWidth) > (simulationWidth-wallRepulsionDistance)) {
			tweets[i].force.x += ( (simulationWidth-wallRepulsionDistance) - (tweets[i].pos.x + tweets[i].totalWidth) ) * wallRepulsionAtten * tweets[i].deathAttenuation;
		}

		if (tweets[i].pos.y < wallRepulsionDistance) {
			tweets[i].force.y += (wallRepulsionDistance - tweets[i].pos.y) * wallRepulsionAtten * tweets[i].deathAttenuation;
		}
		if ((tweets[i].pos.y + tweets[i].totalHeight) > (simulationHeight-wallRepulsionDistance)) {
			tweets[i].force.y += ( (simulationHeight-wallRepulsionDistance)  - (tweets[i].pos.y + tweets[i].totalHeight)) * wallRepulsionAtten * tweets[i].deathAttenuation;
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
					ofVec2f force = (awayFromOther * (tweetRepulsionDistance - distance) * tweetRepulsionAtten);
					force.y *= yForceBias;
					tweets[i].force += force;
				}
			}
		}
	}
	
	for(int i = 0; i < tweets.size(); i++){
		float f =  fluidForceScale * tweetLayerOpacity * tweets[i].deathAttenuation;
		printf("Fluidforcescale: %f\n", fluidForceScale);
		printf("TweetLayerOpacity: %f\n", tweetLayerOpacity);
		printf("DeathAttenuation: %f\n", tweets[i].deathAttenuation);
		printf("Tweets[i].force.x: %f\n", tweets[i].force.x);
		printf("Tweets[i].force.y: %f\n", tweets[i].force.y);
		printf("F: %f\n", f);
		printf("--------------------------------------------------------\n");

		tweets[i].force.scale(1.4);

		fluidRef->applyForce(
			tweets[i].pos/ofVec2f(simulationWidth,simulationHeight)
			,tweets[i].force/(ofVec2f(simulationWidth,simulationHeight)) * f );
		//fluidRef->applyForce( tweets[i].pos/ofVec2f(simulationWidth,simulationHeight), tweets[i].force/ofVec2f(simulationWidth,simulationHeight) * max(0.1f, fluidForceScale * tweetLayerOpacity * tweets[i].deathAttenuation) );
	}
	
	for(int i = 0; i < tweets.size(); i++){
		tweets[i].update();
	}
	//////////////
	
	searchTermSelected = false;
	selectedSearchTerm = -1;
	for(int i = 0; i < searchTerms.size(); i++){
		if(searchTerms[i].selected){
			searchTermSelected = true;
			selectedSearchTerm = i;
			break;
		}
	}
	
	if(!searchTermSelected){	
		for(int i = 0; i < searchTerms.size(); i++){
			float closestDistance = 99999;
			map<int,KinectTouch>::iterator it;
			for(it = blobsRef->begin(); it != blobsRef->end(); it++){
				ofVec2f point = ofVec2f(it->second.x*simulationWidth, it->second.y*simulationHeight);
				float distance = point.distanceSquared(searchTerms[i].pos);
				if(distance < closestDistance){
					closestDistance = distance;
					searchTerms[i].closestPoint = point;
				}
				searchTerms[i].touchPresent = true;
			}
			
			searchTerms[i].update();
		}
	}	
}

void ofxWWTweetParticleManager::renderTweets(){
	for(int i = 0; i < tweets.size(); i++){
		tweets[i].draw();
	}
}

void ofxWWTweetParticleManager::renderSearchTerms(){	
	for(int i = 0; i < searchTerms.size(); i++){
		if( !searchTermSelected || (searchTermSelected && i == selectedSearchTerm) ){
			searchTerms[i].draw();
		}
	}	
}

void ofxWWTweetParticleManager::onStatusUpdate(const rtt::Tweet& tweet){
	ofxWWTweetParticle tweetParticle;
	tweetParticle.manager = this;
	tweetParticle.pos = ofVec2f(ofRandom(simulationWidth-wordWrapLength), ofRandom(simulationHeight));
	tweetParticle.setTweet(tweet);
	tweets.push_back( tweetParticle );	
	cout << "added tweet " << tweets.size() << " text is " << tweet.getText() << endl;
}

void ofxWWTweetParticleManager::onNewSearchTerm(TwitterAppEvent& event) {
	printf("Yay I got a new search term: %s\n", event.search_term.c_str());
	
	ofxWWSearchTerm searchTerm;
	int tries = 0;
	ofVec2f pos;
	bool validPosFound;
	do {
		validPosFound = true;
		pos = ofVec2f(ofRandom(simulationWidth-wordWrapLength/2.), ofRandom(simulationHeight));
		for(int i = 0; i < searchTerms.size(); i++){
			if (searchTerm.pos.distance(pos) < 400) {
				validPosFound = false;
				break;
			}
		}
	} while (!validPosFound && tries++ < 100);
	searchTerm.manager = this;
	searchTerm.term = event.search_term;
	searchTerm.pos = pos;
	
	searchTerms.push_back( searchTerm );	
}

void ofxWWTweetParticleManager::onStatusDestroy(const rtt::StatusDestroy& destroy){
}

void ofxWWTweetParticleManager::onStreamEvent(const rtt::StreamEvent& event){
}
