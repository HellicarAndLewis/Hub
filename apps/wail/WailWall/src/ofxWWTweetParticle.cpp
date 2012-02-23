#include "ofxWWTweetParticle.h"
#include "ofxWWTweetParticleManager.h"
#include "Colours.h"


ofImage *ofxWWTweetParticle::dotImages[NUM_DOT_IMAGES] = {NULL, NULL, NULL, NULL, NULL};



ofImage* ofxWWTweetParticle::highlightImage = NULL;


ofxWWTweetParticle::ofxWWTweetParticle(){
	delete_id = 0;
	whichImage = ofRandom(0, 1);
	if(whichImage<0.5) whichImage = 0;
	else whichImage = ofMap(whichImage, 0.5, 1, 0, 1);
	imageScale = ofRandom(0.5, 2.3);
	manager = NULL;
	isTwoLines = false;
	isSearchTweet = false;
	speedAdjust = 0;

	state = STATE_DEFAULT;
	dot_opacity = 1.0;
	highlight_duration = 300;
	
	if(dotImages[0]==NULL) {
		for(int i = 0; i < NUM_DOT_IMAGES; i++) {
			dotImages[i] = new ofImage();
			dotImages[i]->loadImage("images/td"+ofToString(i)+".png");
		}
	}
	
	if(highlightImage == NULL) {
		highlightImage = new ofImage();
		highlightImage->loadImage("images/highlight.png");
		//highlightImage->setAnchorPercent(0.5, 0.5);
	}
}

void ofxWWTweetParticle::setTweet(rtt::Tweet tw){
	lastPos = pos;
	createdTime = ofGetElapsedTimef();
	this->tweet = tw;
	scale = 1.0;
	opacity = 1.0;
	dead = false;
	isTwoLines = false;
	useBurstOne = ofRandomuf() > .8;
	lineOne = "";
	lineTwo = "";
	lineOneWidth = lineOneHeight = lineTwoWidth = lineTwoHeight = 0;
	
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
//	printf("SET PARTICLE WITH TEXT: %s\n", this->tweet.getText().c_str());
//	totalWidth  = MAX(lineOneWidth,lineTwoWidth);
//	totalHeight = userNameHeight + atSignHeight + lineOneHeight + lineTwoHeight;
}

void ofxWWTweetParticle::update(){

	
	lastPos = pos;
	force.rotate(ofSignedNoise(pos.x/manager->tweetRotateDamp,pos.y/manager->tweetRotateDamp,ofGetElapsedTimef()/manager->tweetChaosSpeed)*manager->tweetRotateAmp );
	pos += force*ofMap(whichImage, 0, 2, 0.6, 1.4);
	//force.set(0,0);
	force *= 0.0;
	
	//birth attenuation just to stop snapping on
	opacity = ofMap(ofGetElapsedTimef(), createdTime, createdTime+.5, .0, 1.0, true);

	//interaction tweet layer attenuation
		

	
	pos += ofVec2f(ofSignedNoise(pos.y/manager->tweetFlowDamp, ofGetElapsedTimef()/manager->tweetChaosSpeed) * (.9-clampedSelectionWeight)*manager->tweetFlowAmp, 
				   ofSignedNoise(pos.x/manager->tweetFlowDamp, ofGetElapsedTimef()/manager->tweetChaosSpeed) * (.9-clampedSelectionWeight)*manager->tweetFlowAmp);
	 
	//death attenuation
	deathAttenuation = ofMap(ofGetElapsedTimef(), createdTime+manager->startFadeTime, createdTime+manager->startFadeTime+manager->fadeDuration, 1.0, 0.0, true);

	if(deathAttenuation == 0) {
		dead = true;
	}
	
	// smoothe the clampedSelectionWeight
	smoothedSelectionWeight.setTarget(clampedSelectionWeight);
	smoothedSelectionWeight.update();
	
	//distance attenuation
	opacity *= ofMap(smoothedSelectionWeight.getValue(), .2, 1.0, 0, 1.0, true); 
	//opacity = 1;
	
	//eventually we can optimize with this:
	if(selectionWeight > 0){
		recalculateBoundingRects();
	}
}


void ofxWWTweetParticle::drawStarImage(float alpha) {
	int firstImage = floor(whichImage);
	int secondImage = ceil(whichImage);
	float amt = whichImage - firstImage;
	glColor4f(1, 1, 1, (1.f - amt)*alpha);
	dotImages[firstImage]->draw(pos.x+manager->dotShift, pos.y, dotImages[firstImage]->getWidth()*imageScale, dotImages[firstImage]->getHeight()*imageScale);
	
	glColor4f(1, 1, 1, amt*alpha);
	dotImages[secondImage]->draw(pos.x+manager->dotShift, pos.y, dotImages[secondImage]->getWidth()*imageScale, dotImages[secondImage]->getHeight()*imageScale);
	

}
// TODO we can change the anchor percentage right? instead of calling each ofSetRectMode()
void ofxWWTweetParticle::drawDot(){
	if(state == STATE_DEFAULT) {

		float alpha = 1;
		alpha *= ofMap(smoothedSelectionWeight.getValue(), 0, .5, 1.0, 0, true);
		
		ofPushStyle();
			//glColor4f(1,1,1,dot_opacity);
			ofSetRectMode(OF_RECTMODE_CENTER);
			alpha *= ofMap(whichImage, 0, 1, .6, 1);
			drawStarImage(alpha);
		ofPopStyle();
	}
	else if(state == STATE_HIDING) {
		ofPushStyle();
			ofSetRectMode(OF_RECTMODE_CENTER);			
			drawStarImage(dot_opacity);
		ofPopStyle();
		
	}
	else if(state == STATE_HIGHLIGHT) {
		
		float now = ofGetElapsedTimeMillis();
		float diff = highlight_duration - (lifetime - now);		
		float p = MIN(1.0f, diff/highlight_duration);
		p = sin(p * PI);
	
		ofPushStyle();
		{
			if(!isDrawingText()) {
				ofSetRectMode(OF_RECTMODE_CENTER);
				
				drawStarImage(1);
				
				glColor4f(1,1,1,p);
				highlightImage->draw(pos.x + manager->dotShift, pos.y);
			}
		}
		ofPopStyle();
	}
}

void ofxWWTweetParticle::drawText(){

	if(!isDrawingText()) {
		return;
	}

	

	ofColor atcolor = ofColor::fromHex(Colours::get(AT_SIGN));
	
	// square root makes the alpha brighter at lower values (0-1)
	atcolor.a = sqrt(opacity)*255;
	ofSetColor(atcolor);
	//DRAW @ 
	ofVec2f atPos = getAtDrawPos();
	
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	manager->sharedUserFont.drawString("@",atPos.x,atPos.y);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	ofColor fontcolor = ofColor::fromHex(Colours::get(LAYER_1_FONT));
	fontcolor.a = atcolor.a;
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

	// Draw the ID of this tweet so we can remove it.
	stringstream ss;
	ss << delete_id;
	manager->sharedTweetFont.drawString(ss.str().c_str(), pos.x, pos.y);

	
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
	return ofVec2f(pos.x - userNameWidth/2, pos.y + manager->userNameYOffset * typePlacementTweenPos());
}

ofVec2f ofxWWTweetParticle::getTweetLineOneDrawPos(){
	return ofVec2f(pos.x - lineOneWidth/2, pos.y + manager->tweetYOffset * typePlacementTweenPos());
}

ofVec2f ofxWWTweetParticle::getTweetLineTwoDrawPos(){
	return ofVec2f(pos.x - lineTwoWidth/2, pos.y + manager->tweetYOffset*typePlacementTweenPos() + lineOneHeight + manager->tweetLineSpace);
}
	
ofVec2f ofxWWTweetParticle::getAtDrawPos(){
	return ofVec2f(pos.x - atSignWidth, pos.y + atSignHeight/2);//ADD SHIFT
}

float ofxWWTweetParticle::typePlacementTweenPos(){
	return isSearchTweet ? 1.0 : ofMap(smoothedSelectionWeight.getValue(),0,.3,0,1.0,true);
}

