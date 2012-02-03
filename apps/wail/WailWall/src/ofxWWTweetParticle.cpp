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
	lastPos = pos;
	createdTime = ofGetElapsedTimef();
	this->tweet = tweet;
	scale = 1.0;
	opacity = 1.0;
	dead = false;
	
	//split tweet up
	float tweetWidth = manager->sharedFont.getStringBoundingBox(tweet.getText(), 0, 0).width;
	if(tweetWidth > manager->wordWrapLength){
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
			
		} while(ss.good());

		wordWrappedTweet = lineOne + "\n" + lineTwo;
		userNameWidth = manager->sharedLargeFont.getStringBoundingBox("@"+tweet.getScreenName(), 0, 0).width;
		totalWidth = userNameWidth + manager->userNameXPad + wrapPoint;
		totalHeight = MAX(manager->sharedLargeFont.getStringBoundingBox(tweet.getScreenName(), 0, 0).height,
						  manager->sharedFont.getStringBoundingBox(wordWrappedTweet, 0, 0).height);
		if(!isTwoLines){
			ofLogError("ofxWWTwitterParticle -- word wrapped didn't hit two lines ... ");
		}
	}
	else{
		wordWrappedTweet = tweet.getText();
		userNameWidth = manager->sharedFont.getStringBoundingBox("@"+tweet.getScreenName(), 0, 0).width;
		totalWidth = userNameWidth + manager->userNameXPad + tweetWidth;
		totalHeight = manager->sharedFont.getStringBoundingBox("@"+tweet.getScreenName(), 0, 0).height;
	}	
}

void ofxWWTweetParticle::update(){
	
	lastPos = pos;
	pos += force;
	force = ofVec2f(0,0);
	
	opacity = ofMap(ofGetElapsedTimef(), createdTime+5, createdTime+10, 1.0, 0.0, true);
	if (opacity == 0) {
		dead = true;
	}
}

void ofxWWTweetParticle::draw(){
	ofPushStyle();
	ofEnableAlphaBlending();
	ofSetColor(255, 255, 255, opacity*255);
	
	if(isTwoLines){
		manager->sharedLargeFont.drawString("@"+tweet.getScreenName(), pos.x, pos.y+manager->userNameYOffset);
	}
	else{
		manager->sharedFont.drawString("@"+tweet.getScreenName(), pos.x, pos.y+manager->userNameYOffset);
	}
	manager->sharedFont.drawString(wordWrappedTweet, pos.x + userNameWidth + manager->userNameXPad, pos.y);
	
	//ofDrawBitmapString(tweet.getText(), pos);
	ofPopStyle();
}