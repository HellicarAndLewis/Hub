/*
 *  ofxWWTweetManager.cpp
 *  WailWall
 *
 *  Created by James George on 1/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxWWRenderer.h"
#include "ofxWWTweetParticleManager.h"
#include "ofxWebSimpleGuiToo.h"

ofxWWTweetParticleManager::ofxWWTweetParticleManager()
	:shouldChangeSearchTermOn(0)
	,changeSearchTermDelay(10)  
	,currentSearchTermIndex(0)
	,renderer(NULL)
{
	maxTweets = 100;
	tweetSearchEndedTime = 0;
	isDoingSearch = false;
}

void ofxWWTweetParticleManager::setup(ofxWWRenderer* ren){
	renderer = ren;
	
	// Initialize twitter.	
	// -------------------
	cout << "init twitter" << endl;
	twitter.init(4444);
	twitter.addDefaultListener();
	twitter.addCustomListener(*this);
	
	cout << "connect twitter" << endl;
	
	if(!twitter.connect()) {
		printf("Error: cannot connect to twitter stream.\n");
	}
	twitter.addListener(this, &ofxWWTweetParticleManager::onNewSearchTerm);

	cout << "adding unused terms" << endl;
	// Get previously received search terms.
	// -------------------------------------
	vector<TwitterSearchTerm*> stored_search_terms;
	if(twitter.getUnusedSearchTerms(stored_search_terms)) {
		vector<TwitterSearchTerm*>::iterator it = stored_search_terms.begin();
		while(it != stored_search_terms.end()) {
			addSearchTerm((*it)->user, (*it)->search_term);
			++it;
		}
	}
	
	burstOne.loadImage("images/burst1.png");
	burstTwo.loadImage("images/burst2.png");
	
	canSelectSearchTerms = false;
	enableCaustics = true;
	
	//fakin' it
	ofAddListener(ofEvents.keyPressed, this, &ofxWWTweetParticleManager::keyPressed);
	fakeSearchTerms.push_back("POLITICS");
	fakeSearchTerms.push_back("ECONOMY");
	fakeSearchTerms.push_back("BIOLOGY");
	fakeSearchTerms.push_back("TED");
	fakeSearchTerms.push_back("DEWARDS");
	fakeSearchTerms.push_back("TECHNOLOGY");
	fakeSearchTerms.push_back("SPIRTUALITY");
	fakeSearchTerms.push_back("CULTURE");

	for(int i = 0; i < fakeSearchTerms.size(); i++){
		twitter.simulateSearch(fakeSearchTerms[i]);
	}
	
	// test pbo
	glGenBuffers(1,&pbo);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
	
	int w = renderer->getFbo().getWidth();
	int h = renderer->getFbo().getHeight();
	int size = w * h * 4;
	glBufferData(GL_PIXEL_PACK_BUFFER, size, NULL, GL_STATIC_READ);

}

void ofxWWTweetParticleManager::keyPressed(ofKeyEventArgs& args) {
	if(args.key == '!'){
		int size = fakeSearchTerms.size();
		int dx = ofRandom(size);	
		string term = fakeSearchTerms.at(dx);
		printf("Using fake search term: %s\n", term.c_str());		
		addSearchTerm("no_user", term );
	}
	else if(args.key == '@') {
		// test screenshot
		printf("trigger screenshot!!\n");
		addCurrentRenderToScreenshotQueue();
	}
}

void ofxWWTweetParticleManager::setupGui(){
	
	webGui.addPage("Caustics");
	webGui.addToggle("Enable Caustics", enableCaustics);
	
	webGui.addPage("Tweet Lifecycle");
	webGui.addSlider("Max Tweets", maxTweets, 5, 500);
	webGui.addToggle("Clear Tweets", clearTweets);
	//webGui.addSlider("Start Fade Time", startFadeTime, 2, 10);
	//webGui.addSlider("Fade Duration", fadeDuration, 2, 10);

	webGui.addPage("Tweet Animation");
	webGui.addToggle("Flow Sideways", tweetsFlowLeftRight);
	webGui.addSlider("Flow Speed", tweetFlowSpeed, -10, 10);	
	webGui.addSlider("Flow Chaos", flowChaosScale, 0, 10);
	
	webGui.addPage("Tweet Appearance");
	webGui.addSlider("Tweet Font Size", fontSize, 5, 124);

	webGui.addSlider("Word Wrap Length", wordWrapLength, 100, 300);
	webGui.addSlider("Dot Size", dotSize, 5, 50);
	webGui.addSlider("Two Line Scale", twoLineScaleup, 1.0, 2.0);
	webGui.addSlider("User Y Shift", userNameYOffset, -150, 150);
	webGui.addSlider("Tweet Y Shift", tweetYOffset, -150, 150);
	webGui.addSlider("Wall Repulsion Dist", wallRepulsionDistance, 0, 900);
	webGui.addSlider("Wall Repulsion Atten", wallRepulsionAtten, 0, .2);
	webGui.addSlider("Tweet Repulsion Dist", tweetRepulsionDistance, 0, 900);
	webGui.addSlider("Tweet Repulsion Atten", tweetRepulsionAtten, 0, .2);
	webGui.addSlider("Y Force Bias", yForceBias, 1., 10.);
	webGui.addSlider("Fluid Force Scale", fluidForceScale, 1., 100.);
	
	webGui.addPage("Search Terms");
	webGui.addSlider("Search Font Size", searchTermFontSize, 40, 70);
	webGui.addSlider("Search Min Opacity", searchMinOpacity, 0, .4);
	webGui.addSlider("Touch Min Dist", searchTermMinDistance, 50, 500);
	webGui.addSlider("Touch Min Hold", searchTermMinHoldTime, .5, 3.0);
	webGui.addSlider("Search Duration", tweetSearchDuration, 2, 15);
	webGui.addSlider("Search Time Between", tweetSearchMinWaitTime, 1, 20);
	webGui.addSlider("Search Repulse Dist", searchTermRepulsionDistance, 0, 900);
	webGui.addSlider("Search Repulse Atten", searchTermRepulsionAttenuation, 0, .2);
	webGui.addSlider("Search Hand Attract", searchTermHandAttractionFactor, 0, .2);
	webGui.addToggle("Search Debug", drawSearchDebug);
	
	//TODO set up in XML
	//ONLY CAN HAVE 4 right now
	//least to most common
	causticColors.push_back(ofColor::fromHex(0xf8edc0)); //LIGHT YELLOW
	causticColors.push_back(ofColor::fromHex(0xe35a35)); //BRIGHT ORANGE
	causticColors.push_back(ofColor::fromHex(0xad3e1c)); //MID ORANGE
	causticColors.push_back(ofColor::fromHex(0x500a03)); //DEEP BROWN
	
	printf("After loader: %f\n", fontSize);
}


void ofxWWTweetParticleManager::update(){

	if(clearTweets){
		tweets.clear();
		clearTweets = false;
	}
	
	#ifdef USE_FTGL
		if(!sharedFont.isLoaded() || int(fontSize) != sharedFont.getSize() || int(fontSize*twoLineScaleup) != sharedLargeFont.getSize()){
		
			if(!sharedFont.loadFont("fonts/Tahoma.ttf", fontSize, true, true, false) ||
			   !sharedLargeFont.loadFont("fonts/Tahoma.ttf", fontSize*twoLineScaleup, true, true, false)){
				ofLogError("ofxWWTweetParticleManager::setup() -- couldn't load font!");
			}
		}
		
		if(!sharedSearchFont.isLoaded() || int(searchTermFontSize) != sharedSearchFont.getSize()){
			if(!sharedSearchFont.loadFont("fonts/Tahoma.ttf", searchTermFontSize, true, true, false)){
				ofLogError("ofxWWTweetParticleManager::setup() -- couldn't load font!");
			}
		}
	#else 
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
	#endif
	
	twitter.update();
	
	for(int i = tweets.size()-1; i >= 0; i--){
		//purge dead tweets
		//don't purge tweets this way when flow is on
//		if(tweets[i].dead){
//			tweets.erase(tweets.begin()+i);
//		}
		
		//purge offscreen tweets
		if(tweetsFlowLeftRight){
			if((tweetFlowSpeed <= 0 && tweets[i].pos.x < -wallRepulsionDistance) || 
			   (tweetFlowSpeed >= 0 && tweets[i].pos.x > simulationWidth+wallRepulsionDistance) )
			{
				tweets.erase(tweets.begin()+i);
			}
		}
		else {
			if((tweetFlowSpeed <= 0 && tweets[i].pos.y < -wallRepulsionDistance) || 
			   (tweetFlowSpeed >= 0 && tweets[i].pos.y > simulationHeight+wallRepulsionDistance) )
			{
				tweets.erase(tweets.begin()+i);
			}
		}
	}
	

	if(tweets.size() > maxTweets){
		tweets.resize(maxTweets);
	}
	
	handleSearch();
	
	updateTweets();
	
	updateSearchTerms();
}

void ofxWWTweetParticleManager::handleSearch() {
	if(isDoingSearch){
		if(ofGetElapsedTimef() - tweetSearchStartTime > tweetSearchDuration){
			finishSearch();
		}
	}
	else {
		if(blobsRef->size() > 0){
			handleTouchSearch();
		}
		else{
			handleTweetSearch();
		}
	}
	
}

void ofxWWTweetParticleManager::handleTouchSearch() {

	bool searchDebug = false;
	if(searchDebug) cout << "++++++ SEARCH DEBUG QUERY " << endl;
	
	//look for a selected search term
	for(int i = 0; i < searchTerms.size(); i++){
		
		if(searchTerms[i].selected){
			searchForTerm(searchTerms[i]);
			selectedSearchTermIndex = i;
			shouldTriggerScreenshot = false;
			break;
			
			/*
			if(searchDebug) cout << "++++++ SEARCH DEBUG SELECTED SEARCH TERM " << searchTerms[i].term << endl;

			searchTermSelected = true;
			selectedSearchTerm = i;
			//TODO: Run Real Query
			vector<rtt::Tweet> result;
			twitter.getTweetsNewerThan(1000000, 25, result);
			if(searchDebug) cout << "++++++ SEARCH DEBUG SELECTED SEARCH TERM RETURNED: " << result.size() << endl;
			if(result.size() != 0){
				if(searchDebug) cout << "++++++ SEARCH DEBUG FOUND REAL RESULTS: " << endl;
				for(int t = 0; t < result.size(); t++){
					if(t < tweets.size()){
						//reappropriate tweet
						if(searchDebug) cout << "++++++ REAPPROPRIATING TWEET" << endl;
						//tweets[t].setTweet( result[t] );
						tweets[t].isSearchTweet = true;
					}
					else{
						if(searchDebug) cout << "++++++ GENERATING TWEET" << endl;
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
				if(searchDebug) cout << "++++++ SEARCH DEBUG CREATING FAKED RESULTS: " << endl;
				for(int t = 0; t < MIN(15, tweets.size()); t++){
					tweets[t].isSearchTweet = true;
					tweets[t].createdTime = ofGetElapsedTimef() + ofRandom(2);
				}
			}
			break;
			*/
		}
	}
	
}

void ofxWWTweetParticleManager::handleTweetSearch(){
	
	if(ofGetElapsedTimef() - tweetSearchEndedTime < tweetSearchMinWaitTime){
		return;
	}
	
	if(!incomingSearchTerms.empty()){
		ofxWWSearchTerm term = incomingSearchTerms.front();
		incomingSearchTerms.pop();

		searchForTerm( term );
		shouldTriggerScreenshot = true;
		selectedSearchTermIndex = searchTerms.size();
		searchTerms.push_back(term);
	}
	//DO IDLE MODE SEARCHING	
	else{
		//DO IDLE MODE SEARCHING
		// TODO: maybe add a count member somehere?
		if(!searchTerms.size()) {
			return;
		}
		
		// When do we need to change to the next search term:
		// --------------------------------------------------
		int now = ofGetElapsedTimef();
		bool change = false;
		if(shouldChangeSearchTermOn == 0) {
			shouldChangeSearchTermOn = now + changeSearchTermDelay;
			change = true;
		}
		else if(now > shouldChangeSearchTermOn) {
			shouldChangeSearchTermOn = now + changeSearchTermDelay;
			printf("Current search term index: %d\n", currentSearchTermIndex);
			currentSearchTermIndex = (++currentSearchTermIndex) % searchTerms.size();
			printf("Next search term index: %d\n", currentSearchTermIndex);
			change = true;
		}
		
		// When we need to change, make the next search term active, and load tweets.
		// --------------------------------------------------------------------------
		if(change) {
			searchForTerm( searchTerms[currentSearchTermIndex] );
			shouldTriggerScreenshot = false;
			selectedSearchTermIndex = currentSearchTermIndex;
		}
	}
}

void ofxWWTweetParticleManager::searchForTerm(ofxWWSearchTerm& term){
	
	cout << "Searching for " << term.term << " by " << term.user << endl;
	
	twitter.setSearchTermAsUsed(term.user, term.term);
	
	vector<rtt::Tweet> found_tweets;
	if(twitter.getTweetsWithSearchTerm(term.term, 100000, 100, found_tweets)) {
		for(int i = 0; i < found_tweets.size(); ++i) {
			printf("[found] %s\n", found_tweets[i].getText().c_str());
			// TODO: This is where we need to create or fill a new tweet. @James lets talk about this
		}
	}
	else {
		//no tweets, just reclaim some random ones

		for(int t = 0; t < 15; t++){
			int randomTweet = ofRandom(tweets.size()-1);
			tweets[randomTweet].isSearchTweet = true;
			tweets[randomTweet].createdTime = ofGetElapsedTimef() + ofRandom(2);
			/*
				tweets[t].isSearchTweet = true;
				tweets[t].createdTime = ofGetElapsedTimef() + ofRandom(2);
			 */
		}
	}
	
	tweetSearchStartTime = ofGetElapsedTimef();
	isDoingSearch = true;

}

void ofxWWTweetParticleManager::finishSearch(){
	
	if(shouldTriggerScreenshot){
		addCurrentRenderToScreenshotQueue();
		shouldTriggerScreenshot = false;
	}
	
	isDoingSearch = false;
	shouldTriggerScreenshot = false;
	for(int i = 0; i < searchTerms.size(); i++){
		searchTerms[i].selected = false;
	}
	tweetSearchEndedTime = ofGetElapsedTimef();
}

void ofxWWTweetParticleManager::addCurrentRenderToScreenshotQueue() {
		glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
// TODO: trigger screenshot of current layout
		int now = ofGetElapsedTimeMillis();
		// %Y-%m-%d-%H-%M-%S-%i
		string dirname = "thumbs/" +ofGetTimestampString("%m-%d");
		ofDirectory dir(dirname);
		dir.create(true);

		string filename = ofGetTimestampString() +"_" +ofToString(ofGetFrameNum()) +".png";
		string filepath(dirname);	
		filepath.append("/");
		filepath.append(filename);

		// pretty sure we can do this better
		ofPixels pixels;
		
		renderer->getFbo().getTextureReference().bind();
		glReadPixels(0, 0, renderer->getFbo().getWidth(), renderer->getFbo().getHeight(), GL_BGRA, GL_UNSIGNED_BYTE, 0);
		GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER_ARB,GL_READ_ONLY);
		
		if(ptr) {
			printf("XXXXXXXXXXX \n");
//			ofPixels pix;
//			pix.setFromPixels(ptr, renderer->getFbo().getWidth(), renderer->getFbo().getHeight(), OF_IMAGE_COLOR_ALPHA);
//			ofImage img;
//			img.setFromPixels(pix);
//			img.saveImage("tester.jpg");
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER);			
		}
		//renderer->getFbo().readToPixels(pixels);
		twitter.writeScreenshot(filepath, "roxlu", pixels);
		int end = ofGetElapsedTimeMillis();

		ofPixels pix;
		pix.setFromPixels(ptr, ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);

		printf("It took us: %d millis to add the image to the queue\n", (end-now));

		printf("----------------> adding screenshot <----------------------\n");
		/*
		ofImage img;
		img.setFromPixels(pixels);
		img.saveImage(filepath);

		printf(">>>> upload a photo.\n");
		renderer.getTweetManager().getTwitterApp().uploadScreenshot(ofToDataPath(filepath, true), "roxlu", "");
		*/		

}

float ofxWWTweetParticleManager::weightBetweenPoints(ofVec2f touch, float normalizedSize, ofVec2f tweet){
	float touchMid = normalizedSize*simulationHeight*touchSizeScale;
	return ofMap(touch.distance(tweet), touchMid-touchInfluenceFalloff/2., touchMid+touchInfluenceFalloff/2., 1.0, 0.0, true);
}

void ofxWWTweetParticleManager::updateTweets(){
	
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
	
	
	///ANIMATE tweet
	//apply wall forces
	for(int i = 0; i < tweets.size(); i++){
		if(!tweetsFlowLeftRight){
			//LEFT WALL
			if (tweets[i].pos.x < wallRepulsionDistance) {
				tweets[i].force.x += (wallRepulsionDistance - tweets[i].pos.x) * wallRepulsionAtten * tweets[i].deathAttenuation;
			}
			//RIGHT WALL
			if ((tweets[i].pos.x + tweets[i].totalWidth) > (simulationWidth-wallRepulsionDistance)) {
				tweets[i].force.x += ( (simulationWidth-wallRepulsionDistance) - (tweets[i].pos.x + tweets[i].totalWidth) ) * wallRepulsionAtten * tweets[i].deathAttenuation;
			}
		}
		else {
			//TOP
			if (tweets[i].pos.y < wallRepulsionDistance) {
				tweets[i].force.y += (wallRepulsionDistance - tweets[i].pos.y) * wallRepulsionAtten * tweets[i].deathAttenuation;
			}
			
			//BOTTOM
			if ((tweets[i].pos.y + tweets[i].totalHeight) > (simulationHeight-wallRepulsionDistance)) {
				tweets[i].force.y += ( (simulationHeight-wallRepulsionDistance)  - (tweets[i].pos.y + tweets[i].totalHeight)) * wallRepulsionAtten * tweets[i].deathAttenuation;
			}
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
		
	//apply flow
	for(int i = 0; i < tweets.size(); i++){
		ofVec2f forceVector(0,0);
		if(tweetsFlowLeftRight){
			forceVector.x += (tweetFlowSpeed + tweets[i].speedAdjust) * (1-tweets[i].selectionWeight);
		}
		else{
			forceVector.y += (tweetFlowSpeed + tweets[i].speedAdjust) * (1-tweets[i].selectionWeight);
		}
		//TODO add chaose;
		tweets[i].force += forceVector;
	}
	
	for(int i = 0; i < tweets.size(); i++){
		fluidRef->applyForce(tweets[i].pos/ofVec2f(simulationWidth,simulationHeight), tweets[i].force/ofVec2f(simulationWidth,simulationHeight) * fluidForceScale * tweetLayerOpacity * tweets[i].deathAttenuation );
	}
	
	for(int i = 0; i < tweets.size(); i++){
		tweets[i].update();
	}
}

void ofxWWTweetParticleManager::updateSearchTerms(){
	if(!isDoingSearch){	
		
		//find the closest touch
		for(int i = 0; i < searchTerms.size(); i++){
			searchTerms[i].closestDistanceSquared = 99999;
			map<int,KinectTouch>::iterator it;
			for(it = blobsRef->begin(); it != blobsRef->end(); it++){
				
				ofVec2f point = ofVec2f(it->second.x*simulationWidth, it->second.y*simulationHeight);
				float squareDistance = point.distanceSquared(searchTerms[i].pos);
				if(squareDistance < searchTerms[i].closestDistanceSquared){
					searchTerms[i].closestDistanceSquared = squareDistance;
					searchTerms[i].closestPoint = point;
					searchTerms[i].closestTouchID = it->first;
				}
			}
		}
		
		
		for(int i = 0; i < searchTerms.size(); i++){
			//LEFT WALL
			if (searchTerms[i].pos.x < wallRepulsionDistance) {
				searchTerms[i].force.x += (wallRepulsionDistance - searchTerms[i].pos.x) * wallRepulsionAtten;
			}
			//RIGHT WALL
			if ((searchTerms[i].pos.x) > (simulationWidth-wallRepulsionDistance)) {
				searchTerms[i].force.x += ( (simulationWidth-wallRepulsionDistance) - (searchTerms[i].pos.x) ) * wallRepulsionAtten;
			}
			//TOP
			if (searchTerms[i].pos.y < wallRepulsionDistance) {
				searchTerms[i].force.y += (wallRepulsionDistance - searchTerms[i].pos.y) * wallRepulsionAtten;
			}
			
			//BOTTOM
			if ((searchTerms[i].pos.y) > (simulationHeight-wallRepulsionDistance)) {
				searchTerms[i].force.y += ( (simulationHeight-wallRepulsionDistance)  - searchTerms[i].pos.y) * wallRepulsionAtten;
			}
		}
		
		//now calculate repulsion forces
		float squaredMinDistance = searchTermRepulsionDistance*searchTermRepulsionDistance;
		for(int i = 0; i < searchTerms.size(); i++){
			searchTerms[i].force = ofVec2f(0,0);
			for(int j = 0; j < searchTerms.size(); j++){
				if(i != j){
					float distanceSquared = searchTerms[i].pos.distanceSquared( searchTerms[j].pos );
					if(distanceSquared > squaredMinDistance){
						continue;
					}
					
					float distance = sqrtf(distanceSquared);
					ofVec2f awayFromOther = (searchTerms[i].pos - searchTerms[j].pos)/distance;
					ofVec2f force = (awayFromOther * ((searchTermRepulsionDistance - distance) * searchTermRepulsionAttenuation));
					searchTerms[i].force += force;			
				}
			}
		}
	}
	
	
	for(int i = 0; i < searchTerms.size(); i++){
		searchTerms[i].touchPresent = blobsRef->size() != 0;
		searchTerms[i].update();
	}
}

void ofxWWTweetParticleManager::renderTweets(){
	for(int i = 0; i < tweets.size(); i++){
		if(!tweets[i].isSearchTweet){
			tweets[i].drawText();
			tweets[i].drawDot();
		}
	}
}

void ofxWWTweetParticleManager::renderSearchTerms(){	
	for(int i = 0; i < tweets.size(); i++){
		if(tweets[i].isSearchTweet){
			tweets[i].drawText();
			tweets[i].drawDot();
		}
	}
	
	for(int i = 0; i < searchTerms.size(); i++){
		if( !isDoingSearch || (isDoingSearch && i == selectedSearchTermIndex) ){
			searchTerms[i].draw();
		}
		
		if(drawSearchDebug){
			searchTerms[i].drawDebug();
		}
	}	
	
	if(drawSearchDebug){
		string searchTermDebugString = "";
		searchTermDebugString += "# OF SEARCH TERMS: " + ofToString(searchTerms.size()) + "\n";
		searchTermDebugString += "QUEUE " + ofToString(incomingSearchTerms.size()) + "\n";
		searchTermDebugString += string("IS SEARCHING? ") + (isDoingSearch ? "YES" : "NO") + "\n";
		if(isDoingSearch){
			searchTermDebugString += "SEARCH TERM " + searchTerms[selectedSearchTermIndex].term + "\n";
		}
//		searchTermDebugString += "LAST SEARCH TIME " + (lastSe
		sharedSearchFont.drawString(searchTermDebugString, 30, 30);
	}
}

void ofxWWTweetParticleManager::renderCaustics(){
	if(!enableCaustics){
		return;
	}
	
	ofPushStyle();
	ofSetLineWidth(2);
	
	if(tweetLayerOpacity > 0){
		for(int i = 0; i < tweets.size(); i++){
			for(int j = 0; j < tweets.size(); j++){
				if(j != i){
					attemptCausticConnection(tweets[i].pos, tweets[i].selectionWeight,
											 tweets[j].pos, tweets[j].selectionWeight, tweetLayerOpacity);
				}
			}
		}	
	}
	
	if(isDoingSearch){
		for(int i = 0; i < tweets.size(); i++){
			if(tweets[i].isSearchTweet){
//				cout << "++++++ DRAWING CAUSTICS BETWEEN " << tweets[i].pos << " " << searchTerms[selectedSearchTerm].pos << endl;
				attemptCausticConnection(tweets[i].pos, 1.0, searchTerms[selectedSearchTermIndex].pos, 1.0, 1-tweetLayerOpacity);
			}
		}
	}
	
	ofPopStyle();	
}

void ofxWWTweetParticleManager::attemptCausticConnection(ofVec2f pos1, float weight1, ofVec2f pos2, float weight2, float layerOpacity){
	float chanceOfSynapse = weight1 * weight2;
	if(ofRandomuf() + .5 < chanceOfSynapse){
		setRandomCausticColor(layerOpacity);
		ofLine(pos1, pos2);
	}
}

void ofxWWTweetParticleManager::setRandomCausticColor(float layerOpacity){
	float diceroll = ofRandomuf();
	if(diceroll < .1){
		ofSetColor(causticColors[0], 100*layerOpacity);
	}
	else if(diceroll < .3){
		ofSetColor(causticColors[1], 100*layerOpacity);	
	}
	else if(diceroll < .6){
		ofSetColor(causticColors[2], 100*layerOpacity);		
	}
	else{ 
		ofSetColor(causticColors[3], 100*layerOpacity);	
	}
}


void ofxWWTweetParticleManager::onStatusUpdate(const rtt::Tweet& tweet){
	string bad_word;
	if(twitter.containsBadWord(tweet.getText(), bad_word)) {
		return;
	}
	ofxWWTweetParticle tweetParticle = createParticleForTweet(tweet);
	tweets.push_back( tweetParticle );	
}

ofxWWTweetParticle ofxWWTweetParticleManager::createParticleForTweet(const rtt::Tweet& tweet){
	ofxWWTweetParticle tweetParticle;
	tweetParticle.manager = this;
	if(tweetFlowSpeed == 0){
	}
	else if(tweetsFlowLeftRight){
		//LEFT
		if(tweetFlowSpeed > 0){
			tweetParticle.pos = ofVec2f(ofRandom(-100, -10), ofRandom(-20, simulationHeight+20));			
		}
		//RIGHT
		else{
			tweetParticle.pos = ofVec2f(ofRandom(simulationWidth+10, simulationWidth+100), ofRandom(-20, simulationHeight+20));
		}
	}
	else{
		//TOP
		if(tweetFlowSpeed > 0){
			tweetParticle.pos = ofVec2f(ofRandom(-20, simulationWidth+20), ofRandom(-10, -100));
		}
		//BOTTOM
		else{
			tweetParticle.pos = ofVec2f(ofRandom(-20, simulationWidth+20), ofRandom(simulationHeight+10, simulationHeight+100));
		}
	}
	
	tweetParticle.speedAdjust = ofRandom(-flowChaosScale, flowChaosScale);
	
	//tweetParticle.pos = ofVec2f(ofRandom(simulationWidth-wordWrapLength), ofRandom(simulationHeight));
	tweetParticle.setTweet(tweet);
	return tweetParticle;
}
					
void ofxWWTweetParticleManager::onNewSearchTerm(TwitterAppEvent& event) {
	addSearchTerm(event.tweet.getScreenName(), event.search_term);
}

void ofxWWTweetParticleManager::addSearchTerm(const string& user, const string& term) {
	ofxWWSearchTerm searchTerm;
	searchTerm.pos = ofVec2f(ofRandom(simulationWidth-wordWrapLength/2.), ofRandom(simulationHeight));
	searchTerm.manager = this;
	searchTerm.term = term;
	searchTerm.user = user;
	incomingSearchTerms.push(searchTerm);	
}

void ofxWWTweetParticleManager::onStatusDestroy(const rtt::StatusDestroy& destroy){
}

void ofxWWTweetParticleManager::onStreamEvent(const rtt::StreamEvent& event){
}


TwitterApp& ofxWWTweetParticleManager::getTwitterApp() {
	return twitter;
}
