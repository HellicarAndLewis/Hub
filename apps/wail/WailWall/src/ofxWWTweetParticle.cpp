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
	float tweetWidth = manager->sharedTweetFont.getStringBoundingBox(tweet.getText(), 0, 0).width;
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
			float newWidth = manager->sharedTweetFont.getStringBoundingBox(token, 0, 0).width;
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
		userNameWidth = manager->sharedUserFont.getStringBoundingBox(tweet.getScreenName(), 0, 0).width;
		lineTwoWidth = manager->sharedTweetFont.getStringBoundingBox(lineTwo,0,0).width;
		lineTwoHeight = manager->sharedTweetFont.getStringBoundingBox(lineTwo,0,0).height;
		if(!isTwoLines){
			ofLogError("ofxWWTwitterParticle -- word wrapped didn't hit two lines ... ");
		}
	}
	else {
		//wordWrappedTweet = tweet.getText();
		lineOne = tweet.getText();		
		lineTwoWidth = 0;
		lineTwoHeight = 0;
	}	
	
	lineOneWidth = manager->sharedTweetFont.getStringBoundingBox(lineOne,0,0).width;
	lineOneHeight = manager->sharedTweetFont.getStringBoundingBox(lineOne,0,0).height;
	userNameWidth = manager->sharedUserFont.getStringBoundingBox(tweet.getScreenName(), 0, 0).width;
	
	atSignWidth = manager->sharedUserFont.getStringBoundingBox("@", 0, 0).width;
	atSignHeight = manager->sharedUserFont.getStringBoundingBox("@", 0, 0).height;

//	totalWidth  = MAX(lineOneWidth,lineTwoWidth);
//	totalHeight = userNameHeight + atSignHeight + lineOneHeight + lineTwoHeight;
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
			isSearchTweet = false; 
		}
	}
	else {
		//death attenuation
		deathAttenuation = ofMap(ofGetElapsedTimef(), createdTime+manager->startFadeTime, createdTime+manager->startFadeTime+manager->fadeDuration, 1.0, 0.0, true);
		opacity *= manager->tweetLayerOpacity;
		if(deathAttenuation == 0) {
			dead = true;
		}
		
		//distance attenuation
		opacity *= clampedSelectionWeight; 
	}
	
	//eventually we can optimize with this:
	if(selectionWeight > 0){
		recalculateBoundingRects();
	}
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
		alpha *= ofMap(clampedSelectionWeight, 0, .5, 1.0, 0, true);
	}
	float scale = useBurstOne ? 1.2 : 1.0;
	if(isSearchTweet){
		scale *= 1.5;
	}
	
	if(alpha > 0){
		ofSetColor(255,255,255, alpha*255);
		if(useBurstOne){
			//manager->burstOne.draw(pos.x+manager->dotShift, pos.y, manager->dotSize*1.2,manager->dotSize*1.2);
			ofRect(pos.x+manager->dotShift, pos.y, manager->dotSize*1.2,manager->dotSize*1.2 );
		}
		else{
			//manager->burstTwo.draw(pos.x+manager->dotShift, pos.y, manager->dotSize,manager->dotSize);
			ofRect(pos.x+manager->dotShift, pos.y, manager->dotSize,manager->dotSize);
		}
	}
	
	ofPopStyle();
}

void ofxWWTweetParticle::drawText(){
	
	ofPushStyle();
	ofEnableAlphaBlending();

	ofColor atcolor = manager->atSignColor;
	atcolor.a = opacity*255;
	ofSetColor(atcolor);
	//DRAW @ 
	ofVec2f atPos = getAtDrawPos();
	manager->sharedUserFont.drawString("@",atPos.x,atPos.y);

	ofColor fontcolor = manager->layerOneFontColor;
	fontcolor.a = opacity*255;
	ofSetColor(fontcolor);
	
	//USER -- use the same size as search ftm
	ofVec2f userDrawPos = getUserDrawPos();
	manager->sharedUserFont.drawString(tweet.getScreenName(), userDrawPos.x,userDrawPos.y);
	
	//TWEET
	ofVec2f tweetDrawPos = getTweetLineOneDrawPos();
	manager->sharedTweetFont.drawString(lineOne,tweetDrawPos.x,tweetDrawPos.y);
	if(isTwoLines){
		tweetDrawPos = getTweetLineTwoDrawPos();
		manager->sharedTweetFont.drawString(lineTwo,tweetDrawPos.x,tweetDrawPos.y);		
	}
	
	ofPopStyle();
}

void ofxWWTweetParticle::recalculateBoundingRects(){
	float minx,maxx,miny,maxy;
	ofVec2f userDrawPos = getUserDrawPos();
	ofRectangle userRect = manager->sharedUserFont.getStringBoundingBox(tweet.getScreenName(), userDrawPos.x, userDrawPos.y);
	
	ofVec2f tweetDrawPos = getTweetLineOneDrawPos();
	ofRectangle tweetRect = manager->sharedTweetFont.getStringBoundingBox(lineOne, tweetDrawPos.x, tweetDrawPos.y);
	
	minx = MIN(tweetRect.x,userRect.x);
	maxx = MAX(tweetRect.x+tweetRect.width,userRect.x+userRect.width);
	miny = MIN(tweetRect.y,userRect.y);
	maxy = MAX(tweetRect.y+tweetRect.height,userRect.y+userRect.height);
	if(isTwoLines){
		tweetDrawPos = getTweetLineTwoDrawPos();
		tweetRect = manager->sharedTweetFont.getStringBoundingBox(lineTwo, tweetDrawPos.x, tweetDrawPos.y);
		minx = MIN(tweetRect.x,minx);
		maxx = MAX(tweetRect.x+tweetRect.width,maxx);
		miny = MIN(tweetRect.y,miny);
		maxy = MAX(tweetRect.y+tweetRect.height,maxy);
	}
	
	boundingRect = ofRectangle(minx,miny,maxx-minx,maxy-miny);
	totalWidth = boundingRect.width;
	totalHeight = boundingRect.height;
}

void ofxWWTweetParticle::drawDebug(){
	ofPushStyle();
	ofNoFill();
	ofSetLineWidth(4);
	if(isSearchTweet){
		ofSetColor(0, 255, 255);
	}
	else{
		ofSetColor(255, 255, 255);	
	}
	ofRect(boundingRect);
	ofPopStyle();
}

ofVec2f ofxWWTweetParticle::getBoundingCorner(int cornerIndex){
	if(cornerIndex == 0){
		//return ofVec2f(pos.x-totalWidth/2, pos.y-totalHeight/2);
		return ofVec2f(boundingRect.x,boundingRect.y);
	}
	else if(cornerIndex == 1){
		return ofVec2f(boundingRect.x+boundingRect.width,boundingRect.y);
		//return ofVec2f(pos.x+totalWidth/2, pos.y-totalHeight/2);
	}
	else if(cornerIndex == 2){
		return ofVec2f(boundingRect.x+boundingRect.width,boundingRect.y+boundingRect.height);
		//return ofVec2f(pos.x+totalWidth/2, pos.y+totalHeight/2);
	}
	else if(cornerIndex == 3){
		return ofVec2f(boundingRect.x,boundingRect.y+boundingRect.height);
		//return ofVec2f(pos.x-totalWidth/2, pos.y+totalHeight/2);
	}
	return pos; //shouldn't do this...
}

ofVec2f ofxWWTweetParticle::getUserDrawPos(){
	return ofVec2f(pos.x - userNameWidth/2, pos.y + manager->userNameYOffset * ofMap(clampedSelectionWeight,0,.3,0,1.0,true));
}

ofVec2f ofxWWTweetParticle::getTweetLineOneDrawPos(){
	return ofVec2f(pos.x - lineOneWidth/2, pos.y + manager->tweetYOffset*ofMap(clampedSelectionWeight,0,.3,0,1.0,true));
}

ofVec2f ofxWWTweetParticle::getTweetLineTwoDrawPos(){
	return ofVec2f(pos.x - lineTwoWidth/2, pos.y + manager->tweetYOffset*ofMap(clampedSelectionWeight,0,.3,0,1.0,true) + lineOneHeight + manager->tweetLineSpace);
}
	
ofVec2f ofxWWTweetParticle::getAtDrawPos(){
	return ofVec2f(pos.x - atSignWidth, pos.y + atSignHeight/2);//ADD SHIFT
}

