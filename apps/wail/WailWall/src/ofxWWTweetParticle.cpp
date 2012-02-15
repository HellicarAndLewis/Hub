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
	isSearchTweet = false;
	speedAdjust = 0;
}

void ofxWWTweetParticle::setTweet(rtt::Tweet tweet){
	lastPos = pos;
	createdTime = ofGetElapsedTimef();
	this->tweet = tweet;
	scale = 1.0;
	opacity = 1.0;
	dead = false;
	
	useBurstOne = ofRandomuf() > .8;
	lineOne = "";
	lineTwo = "";

	//split tweet up
	float tweetWidth = manager->sharedFont.getStringBoundingBox(tweet.getText(), 0, 0).width;
	if(tweetWidth > manager->wordWrapLength){
		float curWidth = 0;
		float wrapPoint = tweetWidth/2.0;
		// tokenize
		std::istringstream ss(tweet.getText());
		string token;
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

		//wordWrappedTweet = lineOne + "\n" + lineTwo;
		userNameWidth = manager->sharedLargeFont.getStringBoundingBox(tweet.getScreenName(), 0, 0).width;
//		totalWidth = userNameWidth + manager->userNameXPad + wrapPoint;
//		totalHeight = MAX(manager->sharedLargeFont.getStringBoundingBox(tweet.getScreenName(), 0, 0).height,
//						  manager->sharedFont.getStringBoundingBox(wordWrappedTweet, 0, 0).height);
		lineTwoWidth = manager->sharedFont.getStringBoundingBox(lineTwo,0,0).width;
		if(!isTwoLines){
			ofLogError("ofxWWTwitterParticle -- word wrapped didn't hit two lines ... ");
		}
		
	}
	else {
		//wordWrappedTweet = tweet.getText();
		lineOne = tweet.getText();		
//		totalWidth = userNameWidth + manager->userNameXPad + tweetWidth;
//		totalHeight = manager->sharedFont.getStringBoundingBox(tweet.getScreenName(), 0, 0).height;
	}	
	lineOneWidth = manager->sharedFont.getStringBoundingBox(lineOne,0,0).width;
	lineOneHeight = manager->sharedFont.getStringBoundingBox(lineOne,0,0).height;
	userNameWidth = manager->sharedLargeFont.getStringBoundingBox(tweet.getScreenName(), 0, 0).width;
	
	atSignWidth = manager->sharedLargeFont.getStringBoundingBox("@", 0, 0).width;
	atSignHeight = manager->sharedLargeFont.getStringBoundingBox("@", 0, 0).height;

}

void ofxWWTweetParticle::update(){
	
	lastPos = pos;
	pos += force;
	force = ofVec2f(0,0);
	
	//birth attenuation just to stop snapping on
	opacity = ofMap(ofGetElapsedTimef(), createdTime, createdTime+.5, .0, 1.0, true);

	//interaction tweet layer attenuation
	if(isSearchTweet){
		opacity *= (1-manager->tweetLayerOpacity);
		if(!manager->canSelectSearchTerms){
			isSearchTweet = false; //this will kill the tweet
		}
	}
	else {
		//death attenuation
		deathAttenuation = ofMap(ofGetElapsedTimef(), createdTime+manager->startFadeTime, createdTime+manager->startFadeTime+manager->fadeDuration, 1.0, 0.0, true);
//		opacity *= deathAttenuation; //
		opacity *= manager->tweetLayerOpacity;
		if(deathAttenuation == 0) {
			dead = true;
		}
		
		//distance attenuation
		opacity *= selectionWeight; 
	}
	//opacity = 1.0f;
}

void ofxWWTweetParticle::drawDot(){
	ofPushStyle();

	ofSetRectMode(OF_RECTMODE_CENTER);
	float alpha;
	if(isSearchTweet){
		alpha = 1-manager->tweetLayerOpacity;
	}
	else {
		//alpha = deathAttenuation * manager->tweetLayerOpacity;
		alpha = manager->tweetLayerOpacity;
	}
	float scale = useBurstOne ? 1.2 : 1.0;
	if(isSearchTweet){
		scale *= 1.5;
	}
	
	ofSetColor(255,255,255,alpha*255);
	if(useBurstOne){
		manager->burstOne.draw(pos.x, pos.y, manager->dotSize*1.2,manager->dotSize*1.2);
	}
	else{
		manager->burstTwo.draw(pos.x, pos.y, manager->dotSize,manager->dotSize);
	}

	ofPopStyle();
}

void ofxWWTweetParticle::drawText(){
	
	ofPushStyle();
	ofEnableAlphaBlending();

	//DRAW @ 
	ofSetColor(ofColor::fromHex(0x6f2b1d, opacity*255)); //TODO set font color
	manager->sharedLargeFont.drawString("@", pos.x - atSignWidth/2, pos.y - atSignHeight/2);
	
	ofSetColor(ofColor::fromHex(0xe6ab38, opacity*255)); //TODO set font color
	//USER NAME BELOW
	manager->sharedLargeFont.drawString(tweet.getScreenName(), pos.x - userNameWidth/2, pos.y + manager->userNameYOffset);
	//DRAW TWEET
	manager->sharedFont.drawString(lineOne, pos.x - lineOneWidth/2, pos.y + manager->tweetYOffset);
	if(isTwoLines){
		manager->sharedFont.drawString(lineTwo, pos.x - lineTwoWidth/2, pos.y + manager->tweetYOffset + lineOneHeight);		
	}
	
	ofPopStyle();
}