/*
 *  ofxWWTweetManager.cpp
 *  WailWall
 *
 *  Created by James George on 1/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxWWTweetParticleManager.h"
#include "ofxWebSimpleGuiToo.h"

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
	// roxlu, 02/09: use the twitter_hashtags.txt file
	
	//twitter.track("love");
	//twitter.track("usa");
	//twitter.track("monkey");
	
	if(!twitter.connect()) {
		printf("Error: cannot connect to twitter stream.\n");
	}
	
	burstOne.loadImage("images/burst1.png");
	burstTwo.loadImage("images/burst2.png");

	twitter.addListener(this, &ofxWWTweetParticleManager::onNewSearchTerm);
	canSelectSearchTerms = false;
}

void ofxWWTweetParticleManager::setupGui(){
	
	
	webGui.addPage("Caustics");
	webGui.addToggle("Enable Caustics", enableCaustics);
	
	webGui.addPage("Tweet Lifecycle");
	webGui.addSlider("Tweet Font Size", fontSize, 5, 24);
	webGui.addSlider("Word Wrap Length", wordWrapLength, 100, 300);
	webGui.addSlider("Max Tweets", maxTweets, 5, 100);
	webGui.addSlider("Start Fade Time", startFadeTime, 2, 10);
	webGui.addSlider("Fade Duration", fadeDuration, 2, 10);
	webGui.addToggle("Clear Tweets", clearTweets);

	
	webGui.addPage("Tweet Appearance");
	webGui.addSlider("Dot Size", dotSize, 5, 50);
	webGui.addSlider("Two Line Scale", twoLineScaleup, 1.0, 2.0);
	webGui.addSlider("User Y Shift", userNameYOffset, -150, 150);
//	webGui.addSlider("Two Line Squish", twoLineSquish, .5, 1.0);
	webGui.addSlider("Wall Repulsion Dist", wallRepulsionDistance, 0, 300);
	webGui.addSlider("Wall Repulsion Atten", wallRepulsionAtten, 0, .5);
	webGui.addSlider("Tweet Repulsion Dist", tweetRepulsionDistance, 0, 300);
	webGui.addSlider("Tweet Repulsion Atten", tweetRepulsionAtten, 0, .5);
	webGui.addSlider("Y Force Bias", yForceBias, 1., 10.);
	webGui.addSlider("Fluid Force Scale", fluidForceScale, 1., 100.);
	
	webGui.addPage("Search Terms");
	webGui.addSlider("Search Term Size", searchTermFontSize, 40, 70);
	webGui.addSlider("Search Min Dist", searchTermMinDistance, 50, 500);
	webGui.addSlider("Search Min Hold T", searchTermMinHoldTime, .5, 3.0);

	generateFakeSearchTerms = true;
	
	//TODO set up in XML
	//ONLY CAN HAVE 4 right now
	//least to most common
	causticColors.push_back(ofColor::fromHex(0xf8edc0)); //LIGHT YELLOW
	causticColors.push_back(ofColor::fromHex(0xe35a35)); //BRIGHT ORANGE
	causticColors.push_back(ofColor::fromHex(0xad3e1c)); //MID ORANGE
	causticColors.push_back(ofColor::fromHex(0x500a03)); //DEEP BROWN
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
	
	if(!sharedSearchFont.isLoaded() || searchTermFontSize != sharedSearchFont.getSize()){
		if(!sharedSearchFont.loadFont("fonts/Tahoma.ttf", searchTermFontSize, true, true, false)){
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
	
//	cout << "can select search term ? " << canSelectSearchTerms << " layer opacity? " << tweetLayerOpacity << " search term selected? " << searchTermSelected << endl;
	
	//////////////
	//IF A USER IS PRESENT HANDLE THEM
	if(blobsRef->size() > 0){
		handleTouchSearch();
	}
	else{
		handleTweetSearch();
	}
		
	for(int i = 0; i < searchTerms.size(); i++){
		searchTerms[i].touchPresent = blobsRef->size() != 0;
		searchTerms[i].update();
	}		
	
//	if(searchTermSelected){
	//	updateTweets(searchTweets, 1-tweetLayerOpacity);
//	}
	
	//hand reveal top level tweets
	for(int i = 0; i < tweets.size(); i++){
		tweets[i].selectionWeight = 0;
		map<int,KinectTouch>::iterator it;
		for(it = blobsRef->begin(); it != blobsRef->end(); it++){
			if(tweets[i].selectionWeight < 1){
				ofVec2f touchpoint = ofVec2f(it->second.x*simulationWidth, it->second.y*simulationHeight);
				float weightBetween = weightBetweenPoints(ofVec2f(it->second.x*simulationWidth, it->second.y*simulationHeight), it->second.size, tweets[i].pos);
				tweets[i].selectionWeight = MIN(tweets[i].selectionWeight + weightBetween, 1.0);
			}
		}
	}
	
	updateTweets();
}

void ofxWWTweetParticleManager::handleTouchSearch() {
	
	if(!canSelectSearchTerms){
		searchTermSelected = false;
		return;
	}
	
	if(searchTermSelected){
		return;
	}
	
	//look for a selected search term
	for(int i = 0; i < searchTerms.size(); i++){
		if(searchTerms[i].selected){
			searchTermSelected = true;
			selectedSearchTerm = i;
			if(!searchTerms[i].populated){
				//TODO: Run Real Query
				vector<rtt::Tweet> result;
				twitter.getTweetsNewerThan(1000000, 25, result);
				cout << "query returned " << result.size() << " results " << endl;
				if(result.size() != 0){
					for(int t = 0; t < result.size(); t++){
						if(t < tweets.size()){
							//reappropriate tweet
							tweets[i].setTweet( result[t] );
							tweets[i].isSearchTweet = true;
						}
						else{
							//generate tweet
							ofxWWTweetParticle tweetParticle = createParticleForTweet(result[t]);
							tweetParticle.isSearchTweet = true;
							//searchTweets.push_back( tweetParticle );
							tweets.push_back( tweetParticle );
						}
					}
				}
				//THIS IS A HACK BC QUERY SOMETIMES RETURNS 0
				else {
					for(int t = 0; t < MIN(15, tweets.size()); t++){
						tweets[t].isSearchTweet = true;
						tweets[t].createdTime = ofGetElapsedTimef() + ofRandom(2);
					}
				}
				searchTerms[i].populated = true;
			}
			break;
		}
	}
	
	if(!searchTermSelected){	
		//Update Tweets           
		for(int i = 0; i < searchTerms.size(); i++){
			searchTerms[i].touchPresent = false;
			float closestDistance = 99999;
			map<int,KinectTouch>::iterator it;
			for(it = blobsRef->begin(); it != blobsRef->end(); it++){
				ofVec2f point = ofVec2f(it->second.x*simulationWidth, it->second.y*simulationHeight);
				float distance = point.distanceSquared(searchTerms[i].pos);
				if(distance < closestDistance){
					closestDistance = distance;
					searchTerms[i].closestPoint = point;
				}
			}
		}
	}
	
}

void ofxWWTweetParticleManager::handleTweetSearch(){
	//TODO:
}
	   
float ofxWWTweetParticleManager::weightBetweenPoints(ofVec2f touch, float normalizedSize, ofVec2f tweet){
	float touchMid = normalizedSize*simulationHeight*touchSizeScale;
	return ofMap(touch.distance(tweet), touchMid-touchInfluenceFalloff/2., touchMid+touchInfluenceFalloff/2., 1.0, 0.0, true);
}

void ofxWWTweetParticleManager::updateTweets(){
	
	///ANIMATE tweet
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
		tweets[i].force = ofVec2f(0,0);
		for(int j = 0; j < tweets.size(); j++){
			if(i != j){
				ofVec2f awayFromOther = (tweets[i].pos - tweets[j].pos);
				float distance = awayFromOther.length();
				awayFromOther.normalize();
				if(distance < tweetRepulsionDistance){
					ofVec2f force = (awayFromOther * ((tweetRepulsionDistance - distance) * tweetRepulsionAtten));
					force.y *= yForceBias;
					tweets[i].force += force;
				}
			}
		}
	}
		
	for(int i = 0; i < tweets.size(); i++){
		/*
		 printf("Fluidforcescale: %f\n", fluidForceScale);
		 printf("TweetLayerOpacity: %f\n", tweetLayerOpacity);
		 printf("DeathAttenuation: %f\n", tweets[i].deathAttenuation);
		 printf("tweets[i].force.x: %f\n", tweets[i].force.x);
		 printf("tweets[i].force.y: %f\n", tweets[i].force.y);
		 printf("--------------------------------------------------------\n");		
		 */
		fluidRef->applyForce( tweets[i].pos/ofVec2f(simulationWidth,simulationHeight), tweets[i].force/ofVec2f(simulationWidth,simulationHeight) * fluidForceScale * tweetLayerOpacity * tweets[i].deathAttenuation );
	}
	
	for(int i = 0; i < tweets.size(); i++){
		tweets[i].update();
	}
	
}

void ofxWWTweetParticleManager::renderTweetNodes(){
	for(int i = 0; i < tweets.size(); i++){
		tweets[i].drawDot();
	}
}

void ofxWWTweetParticleManager::renderTweets(){
	for(int i = 0; i < tweets.size(); i++){
		tweets[i].drawText();
	}
}

void ofxWWTweetParticleManager::renderSearchTerms(){	
	for(int i = 0; i < searchTerms.size(); i++){
		if( !searchTermSelected || (searchTermSelected && i == selectedSearchTerm) ){
			searchTerms[i].draw();
		}
	}	
}

void ofxWWTweetParticleManager::renderCaustics(){
	if(!enableCaustics){
		return;
	}
	
	ofPushStyle();
	if(tweetLayerOpacity > 0){
		for(int i = 0; i < tweets.size(); i++){
			for(int j = 0; j < tweets.size(); j++){
				if(j != i){
					attemptCausticConnection(tweets[i].pos, tweets[i].selectionWeight,
											 tweets[j].pos, tweets[j].selectionWeight);
				}
			}
		}	
	}
	
	if(searchTermSelected){
		for(int i = 0; i < tweets.size(); i++){
			if(tweets[i].isSearchTweet){
				attemptCausticConnection(tweets[i].pos, 1.0, searchTerms[selectedSearchTerm].pos, 1.0);
			}
		}
	}
	
	ofPopStyle();	
}

void ofxWWTweetParticleManager::attemptCausticConnection(ofVec2f pos1, float weight1, ofVec2f pos2, float weight2){
	float chanceOfSynapse = weight1 * weight2;
	if(ofRandomuf() + .5 < chanceOfSynapse){
		setRandomCausticColor();
		ofLine(pos1, pos2);
	}
}

void ofxWWTweetParticleManager::setRandomCausticColor(){
	float diceroll = ofRandomuf();
	if(diceroll < .1){
		ofSetColor(causticColors[0], 100*tweetLayerOpacity);
	}
	else if(diceroll < .3){
		ofSetColor(causticColors[1], 100*tweetLayerOpacity);	
	}
	else if(diceroll < .6){
		ofSetColor(causticColors[2], 100*tweetLayerOpacity);		
	}
	else{ 
		ofSetColor(causticColors[3], 100*tweetLayerOpacity);	
	}
}


void ofxWWTweetParticleManager::onStatusUpdate(const rtt::Tweet& tweet){
	if(twitter.containsBadWord(tweet.getText())) {
//		printf("[ censored ] : %s\n", tweet.getText().c_str());
		return;
	}
	
	ofxWWTweetParticle tweetParticle = createParticleForTweet(tweet);
	tweets.push_back( tweetParticle );	
	
//	cout << "added tweet " << tweets.size() << " text is " << tweet.getText() << endl;
}

ofxWWTweetParticle ofxWWTweetParticleManager::createParticleForTweet(const rtt::Tweet& tweet){
	ofxWWTweetParticle tweetParticle;
	tweetParticle.manager = this;
	tweetParticle.pos = ofVec2f(ofRandom(simulationWidth-wordWrapLength), ofRandom(simulationHeight));
	tweetParticle.setTweet(tweet);
	return tweetParticle;
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
	
	
	// @todo using ofSendMessage to test screenshots
	ofSendMessage("take_screenshot");
}


void ofxWWTweetParticleManager::onStatusDestroy(const rtt::StatusDestroy& destroy){
}

void ofxWWTweetParticleManager::onStreamEvent(const rtt::StreamEvent& event){
}


// roxlu 02/07
TwitterApp& ofxWWTweetParticleManager::getTwitterApp() {
	return twitter;
}
