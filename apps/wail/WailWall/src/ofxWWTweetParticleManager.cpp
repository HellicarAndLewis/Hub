#include "ofxWWRenderer.h"
#include "ofxWWTweetParticleManager.h"
#include "ofxWebSimpleGuiToo.h"
#include "Error.h"

ofxWWTweetParticleManager::ofxWWTweetParticleManager()
	:shouldChangeSearchTermOn(0)
	,changeSearchTermDelay(10)  
	,currentSearchTermIndex(0)
	,renderer(NULL)
	,screenshot_userdata(NULL)
{
	maxTweets = 100;
	tweetSearchEndedTime = 0;
	isDoingSearch = false;
	
	tweetSearchMinWaitTime = 10;
	tweetSearchDuration = 5;
	tweetSearchStartTime = 0;
	tweetSearchEndedTime = 0;
}

void ofxWWTweetParticleManager::setup(ofxWWRenderer* ren){
	renderer = ren;
	
	// Initialize twitter.	
	// -------------------
	twitter.init(4444);
	twitter.addDefaultListener();
	twitter.addCustomListener(*this);
	
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
	enableCaustics = false;
	
	ofAddListener(ofEvents.keyPressed, this, &ofxWWTweetParticleManager::keyPressed);
	//fakin' it
	/*
	fakeSearchTerms.push_back("POLITICS");
	fakeSearchTerms.push_back("ECONOMY");
	fakeSearchTerms.push_back("BIOLOGY");
	fakeSearchTerms.push_back("TED");
	fakeSearchTerms.push_back("DEWARDS");
	fakeSearchTerms.push_back("TECHNOLOGY");
	fakeSearchTerms.push_back("LOVE");
	fakeSearchTerms.push_back("CULTURE");

	for(int i = 0; i < fakeSearchTerms.size(); i++){
		twitter.simulateSearch(fakeSearchTerms[i]);
	}
	*/
	setupColors();
}

void ofxWWTweetParticleManager::setScreenshotCallback(takeScreenshotCallback func, void* userdata) {
	screenshot_callback = func;
	screenshot_userdata = userdata;
}

void ofxWWTweetParticleManager::keyPressed(ofKeyEventArgs& args) {
	if(args.key == '!'){
		int size = fakeSearchTerms.size();
		int dx = ofRandom(size);	
		string term = fakeSearchTerms.at(dx);
		printf("Using fake search term: %s\n", term.c_str());		
		addSearchTerm("no_user", term );
	}
}

void ofxWWTweetParticleManager::setupGui(){
	
//	webGui.addPage("Caustics");
//	webGui.addToggle("Enable Caustics", enableCaustics);
//	webGui.addSlider("Caustic Fade", causticFadeSpeed, .002, .5);
	
	webGui.addPage("Tweet Lifecycle");
	webGui.addSlider("Max Tweets", maxTweets, 200, 2000);
	webGui.addToggle("Clear Tweets", clearTweets);
	webGui.addToggle("Draw Tweet Debug", drawTweetDebug);

	webGui.addPage("Tweet Animation");
	webGui.addToggle("Flow Sideways", tweetsFlowLeftRight);
	webGui.addSlider("Flow Speed", tweetFlowSpeed, -10, 10);	
	webGui.addSlider("Flow Chaos", flowChaosScale, 0, 10);
	webGui.addSlider("Wall Repulsion Dist", wallRepulsionDistance, 0, 900);
	webGui.addSlider("Wall Repulsion Atten", wallRepulsionAtten, 0, .2);
	webGui.addSlider("Tweet Repulsion Dist", tweetRepulsionDistance, 0, 900);
	webGui.addSlider("Tweet Repulsion Atten", tweetRepulsionAtten, 0, .2);
	webGui.addSlider("Fluid Force Scale", fluidForceScale, 1., 100.);
	
	webGui.addPage("Tweet Appearance");
	webGui.addSlider("Tweet Font Size", tweetFontSize, 20, 100);
	webGui.addSlider("User Font Size", userFontSize, 20, 100);
	webGui.addSlider("Word Wrap Length", wordWrapLength, 100, 300);
	webGui.addSlider("User Y Shift", userNameYOffset, -150, 150);
	webGui.addSlider("Tweet Y Shift", tweetYOffset, -150, 150);
	webGui.addSlider("Tweet Line Space", tweetLineSpace, 0, 40);
	webGui.addSlider("Dot Size", dotSize, 5, 50);
	webGui.addSlider("Dot Shift", dotShift, -50, 50);
	
	webGui.addPage("Search Terms");
	webGui.addSlider("Max Search Terms", maxSearchTerms, 5, 15);
	webGui.addSlider("Search Font Size", searchTermFontSize, 100, 500);
	webGui.addSlider("Search Min Opacity", searchMinOpacity, 0, .4);
	webGui.addSlider("Touch Min Dist", searchTermMinDistance, 50, 1000);
	webGui.addSlider("Touch Min Hold", searchTermMinHoldTime, .5, 3.0);
	webGui.addSlider("Search Duration", tweetSearchDuration, 2, 15);
	webGui.addSlider("Search Time Between", tweetSearchMinWaitTime, 1, 20);
	webGui.addSlider("Search Repulse Dist", searchTermRepulsionDistance, 500, 2000);
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
	
//	printf("After loader: %f\n", tweetFont);
}


void ofxWWTweetParticleManager::update(){

	twitter.update();
	
	checkFonts();
	
	handleSearch();
	
	updateTweets();
	
	updateSearchTerms();
}

void ofxWWTweetParticleManager::checkFonts(){
	#ifdef USE_FTGL
	if(!sharedTweetFont.isLoaded() || tweetFontSize != sharedTweetFont.getSize()){
		if(!sharedTweetFont.loadFont("fonts/montreal-ttf/Montreal-LightIta.ttf", tweetFontSize, true, true, false)){
			ofLogError("ofxWWTweetParticleManager::setup() -- couldn't load tweet font!");
		}	
		cout << "tweet font allocating! " << tweetFontSize << " " << sharedTweetFont.getSize() << endl;
	}
	
	if(!sharedSearchFont.isLoaded() || searchTermFontSize != sharedSearchFont.getSize()){
		if(!sharedSearchFont.loadFont("fonts/montreal-ttf/Montreal-BoldIta.ttf", searchTermFontSize, true, true, false)){
			ofLogError("ofxWWTweetParticleManager::setup() -- couldn't load search  font!");
		}	
		cout << "search font allocating! " << searchTermFontSize << " " << sharedSearchFont.getSize() << endl;
	}
	
	if(!sharedUserFont.isLoaded() || userFontSize != sharedUserFont.getSize()){
		if(!sharedUserFont.loadFont("fonts/montreal-ttf/Montreal-BoldIta.ttf", userFontSize, true, true, false)){
			ofLogError("ofxWWTweetParticleManager::setup() -- couldn't load search  font!");
		}	
		cout << "user font allocating! " << userFontSize << " " << sharedUserFont.getSize() << endl;
	}
	
	#else 
	//IF WE NEED TO REVERT CORRECT THIS
	//		if(!sharedFont.isLoaded() || fontSize != sharedFont.getSize() || int(fontSize*twoLineScaleup) != sharedLargeFont.getSize()){
	//			if(!sharedFont.loadFont("fonts/Tahoma.ttf", fontSize, true, true, false) ||
	//			   !sharedLargeFont.loadFont("fonts/Tahoma.ttf", fontSize*twoLineScaleup, true, true, false)){
	//				ofLogError("ofxWWTweetParticleManager::setup() -- couldn't load font!");
	//			}
	//		}
	//		
	//		if(!sharedSearchFont.isLoaded() || searchTermFontSize != sharedSearchFont.getSize()){
	//			if(!sharedSearchFont.loadFont("fonts/Tahoma.ttf", searchTermFontSize, true, true, false)){
	//				ofLogError("ofxWWTweetParticleManager::setup() -- couldn't load font!");
	//			}
	//		}
#endif
	
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
		
		if(searchTerms.size() > maxSearchTerms){
			searchTerms[0].dead = true;
			searchTerms[0].killedTime = ofGetElapsedTimef();
		}
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
	if(screenshot_userdata == NULL) {
		return;
	}
	screenshot_callback("roxlutest", screenshot_userdata);
}

float ofxWWTweetParticleManager::weightBetweenPoints(ofVec2f touch, float normalizedSize, ofVec2f tweet){
	float touchMid = normalizedSize*simulationHeight*touchSizeScale;
	return ofMap(touch.distance(tweet), touchMid-touchInfluenceFalloff/2., touchMid+touchInfluenceFalloff/2., 1.0, 0.0, true);
}

void ofxWWTweetParticleManager::updateTweets(){
	
	for(int i = tweets.size()-1; i >= 0; i--){
		//purge offscreen tweets
		if(tweetsFlowLeftRight){
			if((tweetFlowSpeed <= 0 && tweets[i].pos.x < -wallRepulsionDistance) || 
			   (tweetFlowSpeed >= 0 && tweets[i].pos.x > simulationWidth+wallRepulsionDistance) )
			{
				if(tweets.size() > maxTweets){
					tweets.erase(tweets.begin()+i);
				}
				else{
					//wrap around
					tweets[i].pos.x = tweetFlowSpeed > 0 ? -wallRepulsionDistance : simulationWidth + wallRepulsionDistance;
				}
			}
		}
		else {
			if((tweetFlowSpeed <= 0 && tweets[i].pos.y < -wallRepulsionDistance) || 
			   (tweetFlowSpeed >= 0 && tweets[i].pos.y > simulationHeight+wallRepulsionDistance) )
			{
				if(tweets.size() > maxTweets){
					tweets.erase(tweets.begin()+i);
				}
				else{
					//wrap around
					tweets[i].pos.y = tweetFlowSpeed > 0 ? -wallRepulsionDistance : simulationHeight + wallRepulsionDistance;
				}
			}
		}
	}
	
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
				tweets[i].force.x += (wallRepulsionDistance - tweets[i].pos.x) * wallRepulsionAtten;
			}
			//RIGHT WALL
			if ((tweets[i].pos.x) > (simulationWidth-wallRepulsionDistance)) {
				tweets[i].force.x += ( (simulationWidth-wallRepulsionDistance) - (tweets[i].pos.x + tweets[i].totalWidth) ) * wallRepulsionAtten;
			}
		}
		else {
			//TOP
			if (tweets[i].pos.y < wallRepulsionDistance) {
				tweets[i].force.y += (wallRepulsionDistance - tweets[i].pos.y) * wallRepulsionAtten;
			}
			
			//BOTTOM
			if ((tweets[i].pos.y) > (simulationHeight-wallRepulsionDistance)) {
				tweets[i].force.y += ( (simulationHeight-wallRepulsionDistance)  - (tweets[i].pos.y + tweets[i].totalHeight)) * wallRepulsionAtten;
			}
		}
	}
	
	//apply mutual repulsion
	float tweetRepulsionDistanceSqr = tweetRepulsionDistance*tweetRepulsionDistance;
	for(int i = 0; i < tweets.size(); i++){
		for(int j = 0; j < tweets.size(); j++){
			if(i != j){
				ofVec2f awayFromOther = (tweets[i].pos - tweets[j].pos);
				if(awayFromOther.lengthSquared() > tweetRepulsionDistanceSqr){
					continue;
				}
				
				float distance = awayFromOther.length();
				awayFromOther /= distance; //normalize
				ofVec2f force = (awayFromOther * ((tweetRepulsionDistance - distance) * tweetRepulsionAtten));
				tweets[i].force += force;
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
	
	//apply legibility fixes for visible tweets
	for(int i = 0; i < tweets.size(); i++){
		for(int j = 0; j < tweets.size(); j++){
			if(j != i && tweets[i].selectionWeight > .1 && tweets[j].selectionWeight > .1){
				//compare our corners to see if they are in their rect and then apply force if so
				for(int c = 0; c < 4; c++){
					ofVec2f corner = tweets[i].getBoundingCorner(c);
					if(tweets[j].boundingRect.inside( corner )){
						ofVec2f myCenter = ofVec2f(tweets[i].boundingRect.x + tweets[i].totalWidth/2, 
												   tweets[i].boundingRect.y + tweets[i].totalHeight/2);
						ofVec2f otherCenter = ofVec2f(tweets[j].boundingRect.x + tweets[j].totalWidth/2, 
													  tweets[j].boundingRect.y + tweets[j].totalHeight/2);
						if(myCenter.x > otherCenter.x){
							tweets[i].force.x -= (tweets[j].boundingRect.x+tweets[j].boundingRect.width - (myCenter.x+tweets[i].boundingRect.width/2) ) * tweetRepulsionAtten/2.;
						}
						else{
							tweets[i].force.x += (tweets[j].boundingRect.x - (myCenter.x+tweets[i].boundingRect.width/2) ) * tweetRepulsionAtten/2.;
						}
						
						if(myCenter.y > otherCenter.y){
							tweets[i].force.y -= (tweets[j].boundingRect.y+tweets[j].boundingRect.height - (myCenter.y+tweets[i].boundingRect.height/2) ) * tweetRepulsionAtten/2.;
						}
						else{
							tweets[i].force.y += (tweets[j].boundingRect.y - (myCenter.y+tweets[i].boundingRect.height/2) ) * tweetRepulsionAtten/2.;
						}
					}
				}
			}
		}
	}
	
	for(int i = 0; i < tweets.size(); i++){
		fluidRef->applyForce(tweets[i].pos/ofVec2f(simulationWidth,simulationHeight), tweets[i].force/ofVec2f(simulationWidth,simulationHeight) * fluidForceScale * tweetLayerOpacity * tweets[i].deathAttenuation );
	}
	
	for(int i = 0; i < tweets.size(); i++){
		tweets[i].update();
	}
}

void ofxWWTweetParticleManager::updateSearchTerms(){
		
	if(clearTweets){
		tweets.clear();
		clearTweets = false;
	}
	
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
		searchTerms[i].wallForceApplied = false;
		//LEFT WALL
		if (searchTerms[i].pos.x < wallRepulsionDistance) {
			searchTerms[i].force.x += (wallRepulsionDistance - searchTerms[i].pos.x) * wallRepulsionAtten;
			searchTerms[i].wallForceApplied = true;
		}
		//RIGHT WALL
		if ((searchTerms[i].pos.x) > (simulationWidth-wallRepulsionDistance)) {
			searchTerms[i].force.x += ( (simulationWidth-wallRepulsionDistance) - (searchTerms[i].pos.x) ) * wallRepulsionAtten;
			searchTerms[i].wallForceApplied = true;
		}
		//TOP
		if (searchTerms[i].pos.y < wallRepulsionDistance) {
			searchTerms[i].force.y += (wallRepulsionDistance - searchTerms[i].pos.y) * wallRepulsionAtten;
			searchTerms[i].wallForceApplied = true;
		}		
		//BOTTOM
		if ((searchTerms[i].pos.y) > (simulationHeight-wallRepulsionDistance)) {
			searchTerms[i].force.y += ( (simulationHeight-wallRepulsionDistance)  - searchTerms[i].pos.y) * wallRepulsionAtten;
			searchTerms[i].wallForceApplied = true;
		}
	}
	
	//now calculate repulsion forces
	float squaredMinDistance = searchTermRepulsionDistance*searchTermRepulsionDistance;
	for(int i = 0; i < searchTerms.size(); i++){
		if(searchTerms[i].wallForceApplied){
//			continue;
		}
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
	
	for(int i = searchTerms.size()-1; i >= 0; i--){
		if(searchTerms[i].dead && ofGetElapsedTimef() - searchTerms[i].killedTime > searchTermFadeOutTime){
			searchTerms.erase(searchTerms.begin()+i);
		}
	}
	
	for(int i = 0; i < searchTerms.size(); i++){
		searchTerms[i].touchPresent = blobsRef->size() != 0;
		//cout << "accumulated force is " << searchTerms[i].force  << endl;
		searchTerms[i].update();
	}
}

void ofxWWTweetParticleManager::renderTweets(){
	for(int i = 0; i < tweets.size(); i++){
		if(!tweets[i].isSearchTweet){
			tweets[i].drawText();
			tweets[i].drawDot();
			if(drawTweetDebug){
				tweets[i].drawDebug();
			}
		}
	}
}

void ofxWWTweetParticleManager::renderSearchTerms(){	
	for(int i = 0; i < tweets.size(); i++){
		if(tweets[i].isSearchTweet){
			tweets[i].drawText();
			tweets[i].drawDot();
			if(drawTweetDebug){
				tweets[i].drawDebug();
			}
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
		searchTermDebugString + "SEARCH TERMS: " + ofToString(searchTerms.size()) + "    ";
		searchTermDebugString += "QUEUE " + ofToString(incomingSearchTerms.size()) + "    ";
		searchTermDebugString += string("SEARCHING? ") + (isDoingSearch ? "YES" : "NO") + "    ";
		if(isDoingSearch){
			searchTermDebugString += "TERM " + searchTerms[selectedSearchTermIndex].term + "    ";
			searchTermDebugString += "TIME " + ofToString( tweetSearchDuration - (ofGetElapsedTimef() - tweetSearchStartTime), 2) + "    ";
		}
		else{
			searchTermDebugString += "NEXT " + ofToString( tweetSearchMinWaitTime - (ofGetElapsedTimef() - tweetSearchEndedTime), 2) + "    ";
		}
		searchTermDebugString += "TWEETS " + ofToString(tweets.size());
		
		sharedUserFont.drawString(searchTermDebugString, wallRepulsionDistance+20, wallRepulsionDistance);
		ofPushStyle();
		ofNoFill();
		ofRect(wallRepulsionDistance, wallRepulsionDistance, simulationWidth-wallRepulsionDistance*2, simulationHeight-wallRepulsionDistance*2);
		ofPopStyle();
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

void ofxWWTweetParticleManager::setupColors(){
	ofxXmlSettings colors;
	if(colors.loadFile(ofToDataPath("fonts/fontcolor.xml"))){
		atSignColor = ofColor::fromHex( ofHexToInt( colors.getValue("colors:atsign", "0xFFFFFF")) );
		layerOneFontColor = ofColor::fromHex( ofHexToInt( colors.getValue("colors:layerone", "0xFFFFFF")) );
		layerTwoFontColor = ofColor::fromHex( ofHexToInt( colors.getValue("colors:layerone", "0xFFFFFF")) );
	}
	else{
		ofSystemAlertDialog("fonts/fontcolor.xml not found!");
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


