/*
 *  ofxWWTweet.cpp
 *  WailWall
 *
 *  Created by James George on 1/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxWWTweetParticle.h"
#include "ofxWWTweetParticleManager.h"
ofxWWTweetParticle::ofxWWTweetParticle(){
	manager = NULL;
	isTwoLines = false;
}

void ofxWWTweetParticle::setTweet(rtt::Tweet tweet){
	createdTime = ofGetElapsedTimef();
	this->tweet = tweet;
	scale = 1.0;
	opacity = 1.0;
	dead = false;
	
	//split tweet up
	float tweetWidth = manager->sharedFont.getStringBoundingBox(tweet.getText(), 0, 0).width;
	if(tweetWidth > wordWrapLength){
		float curWidth = 0;
		float wrapPoint = tweetWidth/2.0;
		// tokenize
		std::istringstream ss(tweet.getText());
		string token;
		string lineOne = "";
		string lineTwo = "";
		//cout << "processing tweet " << tweet.getText() << endl;
		int wordsPassed = 0;
		do {
			ss >> token;
			float newWidth = manager->sharedFont.getStringBoundingBox(token, 0, 0).width;
//			cout << "	width for " << token << " is " << newWidth << " currently " << curWidth << endl;;
			if(!isTwoLines && (curWidth + newWidth) > wrapPoint){
	//			cout << "wrapped! at word " << wordsPassed << endl;
				isTwoLines = true;
			}
			if(isTwoLines){
				lineTwo += token + " ";
			}
			else{
				lineOne += token + " ";
			}
			curWidth += newWidth;
			wordsPassed++;
			
		} while (ss.good());

		wordWrappedTweet = lineOne;
		if(isTwoLines){
			wordWrappedTweet += "\n" + lineTwo;
		}
	}
	else{
		wordWrappedTweet = tweet.getText();
	}
}

void ofxWWTweetParticle::update(){
	opacity = ofMap(ofGetElapsedTimef(), createdTime+5, createdTime+10, 1.0, 0.0, true);
	if (opacity == 0) {
		dead = true;
	}
}

void ofxWWTweetParticle::draw(){
	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetColor(255, 255, 255, opacity*255);
	ofRect(pos.x, pos.y, 10, 10);
	
	manager->sharedFont.drawString(wordWrappedTweet, pos.x, pos.y);
	
	//ofDrawBitmapString(tweet.getText(), pos);
	ofPopStyle();
}