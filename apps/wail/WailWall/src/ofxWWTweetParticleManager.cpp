#include "ofxWWRenderer.h"
#include "ofxWWTweetParticleManager.h"
#include "ofxWebSimpleGuiToo.h"
#include "Error.h"

ofxWWTweetParticleManager::ofxWWTweetParticleManager()
	:shouldChangeSearchTermOn(0)
	,changeSearchTermDelay(10)  
	,currentSearchTermIndex(-1)
	,renderer(NULL)
	,screenshot_userdata(NULL)
	,current_provider(NULL)
	,stream_provider(NULL)
	,db_provider(NULL)
{
	lastSearchTermTime = 0;
	maxTweets = 100;

	isDoingSearch = false;
	
	tweetSearchMinWaitTime = 10;
	tweetSearchDuration = 5;
	callToActionTime = 5;
	should_take_picture_on = FLT_MAX;
}

void ofxWWTweetParticleManager::setup(ofxWWRenderer* ren){
	renderer = ren;
	
	// Initialize twitter.	
	// -------------------
	twitter.init(4444);
	twitter.addDefaultListener(); 
	//twitter.addTwitterMentionListenerForSearchTerms(this);
	
	if(!twitter.connect()) {
		printf("Error: cannot connect to twitter stream.\n");
	}
	// 
	twitter.addNewSearchTermListener(this, &ofxWWTweetParticleManager::onNewSearchTerm);

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
	setupColors();
	
	// Create tweet providers
	// ----------------------
	stream_provider = new TweetProviderStream(twitter);
	db_provider = new TweetProviderDB(twitter);
	stream_provider->addListener(this);
	db_provider->addListener(this);
	setCurrentProvider(stream_provider);
	twitter.addCustomListener(*stream_provider); // stream provider wants to listen to incoming tweets.
	
	ofAddListener(ofEvents.keyPressed, this, &ofxWWTweetParticleManager::keyPressed);
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
	else if(args.key == '3') {
		printf("Going to stream provider.\n");
		setCurrentProvider(stream_provider);
	}
	else if(args.key == '4') {
		printf("Going to DB provider (and passing the new search term)\n");
		db_provider->fillWithTweetsWhichContainTerm("love");
		setCurrentProvider(db_provider);
	}
}

void ofxWWTweetParticleManager::setupGui(){
	webGui.addPage("Tweet Lifecycle");
	webGui.addSlider("Max Tweets", maxTweets, 200, 2000);
	webGui.addToggle("Clear Tweets", clearTweets);
	webGui.addToggle("Draw Tweet Debug", drawTweetDebug);

	webGui.addPage("Tweet Animation");
	webGui.addToggle("Flow Sideways", tweetsFlowLeftRight);
	webGui.addSlider("Flow Speed", tweetFlowSpeed, -30, 30);	
	webGui.addSlider("Flow Variance", tweetFlowVariance, 0, 10);
	webGui.addSlider("Flow Chaos Speed", tweetChaosSpeed, 10, 400);
	webGui.addSlider("Flow Chaos Amp", tweetFlowAmp, 1, 40);
	webGui.addSlider("Flow Chaos Damp", tweetFlowDamp, 1, 400);
	webGui.addSlider("Flow Rotate Amp", tweetRotateAmp, 0, 40);
	webGui.addSlider("Flow Rotate Damp", tweetRotateDamp, 40, 400);
					 
	webGui.addSlider("Wall Repulsion Atten", wallRepulsionAtten, 0, .2);
	webGui.addSlider("Tweet Repulsion Dist", tweetRepulsionDistance, 0, 900);
	webGui.addSlider("Tweet Repulsion Atten", tweetRepulsionAtten, 0, .2);
	//webGui.addSlider("Fluid Force Scale", fluidForceScale, 1., 100.);
	
	webGui.addPage("Tweet Appearance");
	webGui.addSlider("Tweet Font Size", tweetFontSize, 20, 100);
	webGui.addSlider("User Font Size", userFontSize, 20, 100);
	webGui.addSlider("Word Wrap Length", wordWrapLength, 100, 300);
	webGui.addSlider("User Y Shift", userNameYOffset, -150, 150);
	webGui.addSlider("Tweet Y Shift", tweetYOffset, -150, 150);
	webGui.addSlider("Tweet Line Space", tweetLineSpace, 0, 40);
	webGui.addSlider("Dot Size", dotSize, 5, 50);
	webGui.addSlider("Dot Shift", dotShift, -50, 50);
//	webGui.addHexColor("At Sign Color", atSignColor);
//	webGui.addHexColor("Layer One Font Color", layerOneFontColor);
//	webGui.addHexColor("Layer Two Font Color", layerTwoFontColor
					   
	webGui.addPage("Search Term Timing");
	webGui.addSlider("Max Search Terms", maxSearchTerms, 5, 15);
	webGui.addSlider("Search Font Size", searchTermFontSize, 100, 500);
	webGui.addSlider("Search Min Opacity", searchMinOpacity, 0, .4);
	webGui.addSlider("Touch Min Dist", searchTermMinDistance, 50, 1000);
	webGui.addSlider("Touch Min Hold", searchTermMinHoldTime, .5, 3.0);
	
	// do we need these anymore?
	webGui.addSlider("Search Duration", tweetSearchDuration, 2, 15);
	webGui.addSlider("Search Time Between", tweetSearchMinWaitTime, 1, 20);
	
	
	webGui.addSlider("Call to action time", callToActionTime, 5, 60);
	
	
	
	webGui.addToggle("Search Debug", drawSearchDebug);
	
	webGui.addPage("Search Term Animation");
	webGui.addSlider("Wall Repulsion Dist", wallRepulsionDistance, 0, 900);
	webGui.addSlider("Search Repulse Dist", searchTermRepulsionDistance, 500, 2000);
	webGui.addSlider("Search Repulse Atten", searchTermRepulsionAttenuation, 0, .2);
	webGui.addSlider("Search Hand Attract", searchTermHandAttractionFactor, 0, .1);
	
	//TODO set up in XML ONLY CAN HAVE 4 right now , least to most common
	causticColors.push_back(ofColor::fromHex(0xf8edc0)); //LIGHT YELLOW
	causticColors.push_back(ofColor::fromHex(0xe35a35)); //BRIGHT ORANGE
	causticColors.push_back(ofColor::fromHex(0xad3e1c)); //MID ORANGE
	causticColors.push_back(ofColor::fromHex(0x500a03)); //DEEP BROWN
}


void ofxWWTweetParticleManager::update(){
	twitter.update();
	current_provider->update();
	
	
	checkFonts();
	
	handleSearch();
	
	updateTweets();
	
	updateSearchTerms();
	
	if(ofGetElapsedTimef() > should_take_picture_on) {
		screenshot_callback("joelgethinlewis", screenshot_userdata);
		should_take_picture_on = FLT_MAX;
	}
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
	
}

void ofxWWTweetParticleManager::handleSearch() {
	
	if(blobsRef->size() > 0){
		handleTouchSearch();
	}
		
	handleTweetSearch();

}

void ofxWWTweetParticleManager::handleTouchSearch() {
	
	bool searchDebug = false;
	if(searchDebug) cout << "++++++ SEARCH DEBUG QUERY " << endl;

	int oldSelectedSearchTermIndex = selectedSearchTermIndex;
	selectedSearchTermIndex = -1;
	
	//look for a selected search term
	for(int i = 0; i < searchTerms.size(); i++){
		
		if(searchTerms[i].selected){

			selectedSearchTermIndex = i;
			shouldTriggerScreenshot = false;
			break;
		}
	}
	
	if(oldSelectedSearchTermIndex!=selectedSearchTermIndex) {
		if(selectedSearchTermIndex==-1) {
			setCurrentProvider(stream_provider);
		} else {
			db_provider->fillWithTweetsWhichContainTerm(searchTerms[selectedSearchTermIndex].term);
			setCurrentProvider(db_provider);
		}
	}
	
}

void ofxWWTweetParticleManager::handleTweetSearch(){

	
	// don't allow this to happen too often
	if(ofGetElapsedTimef() - lastSearchTermTime < tweetSearchMinWaitTime){
		return;
	}
	
	lastSearchTermTime = ofGetElapsedTimef();
	
	if(!incomingSearchTerms.empty()){
		ofxWWSearchTerm term = incomingSearchTerms.front();
		incomingSearchTerms.pop();

		shouldTriggerScreenshot = true;
		//selectedSearchTermIndex = searchTerms.size();
		searchTerms.push_back(term);
		
		if(searchTerms.size() > maxSearchTerms){
			searchTerms[0].dead = true;
			searchTerms[0].killedTime = ofGetElapsedTimef();
		}
	}

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
}

void ofxWWTweetParticleManager::addCurrentRenderToScreenshotQueue() {
	if(screenshot_userdata == NULL) {
		return;
	}
	// TODO: add correct username
	screenshot_callback("joelgethinlewis", screenshot_userdata);
}

float ofxWWTweetParticleManager::weightBetweenPoints(ofVec2f touch, float normalizedSize, ofVec2f tweet){
	float touchMid = normalizedSize*simulationHeight*touchSizeScale;
	return MAX( ofMap(touch.distance(tweet), touchMid-touchInfluenceFalloff/2., touchMid+touchInfluenceFalloff/2., 1.0, 0.0, false), 0.0);
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
					//tweets[i].pos.x = tweetFlowSpeed > 0 ? -wallRepulsionDistance : simulationWidth + wallRepulsionDistance;
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
					//tweets[i].pos.y = tweetFlowSpeed > 0 ? -wallRepulsionDistance : simulationHeight + wallRepulsionDistance;
				}
			}
		}
	}
	
	
	map<int,KinectTouch>::iterator it;
	
	//hand reveal top level tweets
	for(int i = 0; i < tweets.size(); i++){
		tweets[i].selectionWeight = 0;
		
		
		for(it = blobsRef->begin(); it != blobsRef->end(); it++){
			float maxRadiusSquared = powf(it->second.z * simulationHeight*touchSizeScale+touchInfluenceFalloff/2, 2.0f);
			ofVec2f touchpoint = ofVec2f(it->second.x*simulationWidth, it->second.y*simulationHeight);
			if(touchpoint.distanceSquared(tweets[i].pos) < maxRadiusSquared){			
				float weightBetween = weightBetweenPoints(touchpoint, it->second.size, tweets[i].pos);
				tweets[i].selectionWeight += weightBetween;
			}
		}
		tweets[i].clampedSelectionWeight = MIN(tweets[i].selectionWeight, 1.0);
	}
	
	///ANIMATE tweet
	//apply wall forces
	for(int i = 0; i < tweets.size(); i++){
		if(!tweetsFlowLeftRight){
			//LEFT WALL
			if (tweets[i].pos.x < 0) {
				tweets[i].force.x += (-tweets[i].pos.x) * wallRepulsionAtten;
			}
			//RIGHT WALL
			if ((tweets[i].pos.x) > (simulationWidth)) {
				tweets[i].force.x += ( simulationWidth - (tweets[i].pos.x) ) * wallRepulsionAtten;
			}
		}
		else {
			//TOP
			if (tweets[i].pos.y < 0) {
				tweets[i].force.y += (-tweets[i].pos.y) * wallRepulsionAtten;
			}
			
			//BOTTOM
			if ((tweets[i].pos.y) > (simulationHeight)) {
				tweets[i].force.y += ( (simulationHeight)  - (tweets[i].pos.y)) * wallRepulsionAtten;
			}
		}
	}
		
	//apply flow
	for(int i = 0; i < tweets.size(); i++){
		if(tweets[i].isSearchTweet){
			continue;
		}
		ofVec2f forceVector(0,0);
		if(tweetsFlowLeftRight){
			forceVector.x += (tweetFlowSpeed + tweets[i].speedAdjust) * (1-tweets[i].clampedSelectionWeight);
		}
		else{
			forceVector.y += (tweetFlowSpeed + tweets[i].speedAdjust) * (1-tweets[i].clampedSelectionWeight);
		}
		tweets[i].force += forceVector;
	}
	
	//apply legibility fixes for visible tweets
	for(int i = 0; i < tweets.size(); i++){
		for(int j = 0; j < tweets.size(); j++){
			if(j != i && tweets[i].selectionWeight > 0 && tweets[j].selectionWeight > 0 &&
						 tweets[i].selectionWeight <= tweets[j].selectionWeight){
				//compare our corners to see if they are in their rect and then apply force if so
				for(int c = 0; c < 4; c++){
					ofVec2f corner = tweets[i].getBoundingCorner(c);
					if(tweets[j].boundingRect.inside( corner )){
						ofVec2f myCenter = ofVec2f(tweets[i].boundingRect.x + tweets[i].totalWidth/2, 
												   tweets[i].boundingRect.y + tweets[i].totalHeight/2);
						ofVec2f otherCenter = ofVec2f(tweets[j].boundingRect.x + tweets[j].totalWidth/2, 
													  tweets[j].boundingRect.y + tweets[j].totalHeight/2);
						if(myCenter.x > otherCenter.x){
							tweets[i].force.x -= (tweets[j].boundingRect.x+tweets[j].boundingRect.width - (myCenter.x+tweets[i].boundingRect.width/2) ) * tweetRepulsionAtten;
						}
						else{
							tweets[i].force.x += (tweets[j].boundingRect.x - (myCenter.x+tweets[i].boundingRect.width/2) ) * tweetRepulsionAtten;
						}
						
						if(myCenter.y > otherCenter.y){
							tweets[i].force.y -= (tweets[j].boundingRect.y+tweets[j].boundingRect.height - (myCenter.y+tweets[i].boundingRect.height/2) ) * tweetRepulsionAtten;
						}
						else{
							tweets[i].force.y += (tweets[j].boundingRect.y - (myCenter.y+tweets[i].boundingRect.height/2) ) * tweetRepulsionAtten;
						}
					}
				}
			}
		}
	}
		
	numSearchTermTweets = 0;
	for(int i = 0; i < tweets.size(); i++){
		tweets[i].update();
		if (tweets[i].isSearchTweet) {
			numSearchTermTweets++;
		}
	}
	
}

void ofxWWTweetParticleManager::updateSearchTerms(){
		
	if(clearTweets){
		tweets.clear();
		clearTweets = false;
	}
	
	
	int closestSearchTerm = -1;
	float closestDistanceSq = FLT_MAX;
	
	//find the closest touch
	if(!blobsRef->empty() && canSelectSearchTerms){
		for(int i = 0; i < searchTerms.size(); i++){
			searchTerms[i].closestDistanceSquared = 9999999;
			map<int,KinectTouch>::iterator it;
			for(it = blobsRef->begin(); it != blobsRef->end(); it++){
				ofVec2f point = ofVec2f(it->second.x*simulationWidth, it->second.y*simulationHeight);
				float squareDistance = point.distanceSquared(searchTerms[i].pos);
				if(squareDistance < searchTerms[i].closestDistanceSquared){
					searchTerms[i].closestDistanceSquared = squareDistance;
					searchTerms[i].closestPoint = point;
					searchTerms[i].closestTouchID = it->first;
				}
				
				
				//printf("tweet %f\n", tweetLayerOpacity);
				// we can choose a selectedSearchTermIndex here
				if(tweetLayerOpacity<=0.2) {
					if(squareDistance < 800*800 && closestDistanceSq>squareDistance) {
					//	printf("hand low enough\n");
						closestSearchTerm = i;
						closestDistanceSq = squareDistance;
//						searchTerms[i].selected = true;
					}
				}
			}
			
			//attract to hand
			ofVec2f directionToHand = searchTerms[i].closestPoint - searchTerms[i].pos;
			float distanceToHand = directionToHand.length();
			if(distanceToHand < searchTermMinDistance){
				searchTerms[i].force += directionToHand * searchTermHandAttractionFactor;
			}
		}
	}
	if(tweetLayerOpacity<=0.2) {
	
		for(int i = 0; i < searchTerms.size(); i++){
			if(i==closestSearchTerm) { 
				
				++searchTerms[i].selected_counter;
				if(searchTerms[i].selected_counter>60) {
					searchTerms[i].selected = true;	
					selectedSearchTermIndex = closestSearchTerm;
					
				}
			} else {
				searchTerms[i].selected = false;
				searchTerms[i].selected_counter = 0;
			}
		}
	} else {
		if(selectedSearchTermIndex!=-1 && searchTerms.size()) {
			searchTerms[selectedSearchTermIndex].selected = true;
			searchTerms[selectedSearchTermIndex].selected_counter = 60;
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
	
	//apply a float and a wobble
	
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
	for(int i = 0; i < searchTerms.size(); i++){
		searchTerms[i].draw();
	}
}

void ofxWWTweetParticleManager::renderConnections(){
	
	ofPushStyle();
	ofSetLineWidth(2);
	
	if(tweetLayerOpacity > 0){
//		cout << "testing connections!!" << endl;
		for(int i = 0; i < tweets.size(); i++){
			if(tweets[i].clampedSelectionWeight <= 0){
				continue;
			}
			
			for(int j = 0; j < tweets.size(); j++){
				if(j != i){
					attemptConnection(tweets[i].pos, tweets[i].clampedSelectionWeight,
									  tweets[j].pos, tweets[j].clampedSelectionWeight, tweetLayerOpacity);
				}
			}
		}	
	}
	
	if(isDoingSearch){
		for(int i = 0; i < tweets.size(); i++){
			if(tweets[i].isSearchTweet){
//				cout << "++++++ DRAWING CAUSTICS BETWEEN " << tweets[i].pos << " " << searchTerms[selectedSearchTerm].pos << endl;
				attemptConnection(tweets[i].pos, 1.0, searchTerms[selectedSearchTermIndex].pos, 1.0, 1-tweetLayerOpacity);
			}
		}
	}
	
	ofPopStyle();	
}

void ofxWWTweetParticleManager::attemptConnection(ofVec2f pos1, float weight1, ofVec2f pos2, float weight2, float layerOpacity){
//	ofLine(pos1, pos2);
//	cout << "drawing line!!" << endl;
	float chanceOfSynapse = weight1 * weight2;
	if(ofRandomuf() + .5 < chanceOfSynapse){
		setRandomCausticColor(layerOpacity);
		ofLine(pos1, pos2);
	}
}

void ofxWWTweetParticleManager::setRandomCausticColor(float layerOpacity){
	
	//JG just returned white for now
	ofSetColor(255);
	return;
	
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

void ofxWWTweetParticleManager::onNewTweet(const rtt::Tweet& tweet) {
	printf(">> [ok] : %s\n", tweet.getText().c_str());	
	ofxWWTweetParticle particle = createParticleForTweet(tweet);
	tweets.push_back(particle);
}
/*
void ofxWWTweetParticleManager::onStatusUpdate(const rtt::Tweet& tweet){
	string bad_word;
	if(twitter.containsBadWord(tweet.getText(), bad_word)) {
		return;
	}
	ofxWWTweetParticle tweetParticle = createParticleForTweet(tweet);
	tweets.push_back( tweetParticle );	
}
*/


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
	
	tweetParticle.speedAdjust = ofRandom(-tweetFlowVariance, tweetFlowVariance);

	//tweetParticle.pos = ofVec2f(ofRandom(simulationWidth-wordWrapLength), ofRandom(simulationHeight));
	tweetParticle.setTweet(tweet);
	return tweetParticle;
}

/*
void ofxWWTweetParticleManager::onNewSearchTermFromPollingAPI(const rtt::Tweet& tweet, const string& term) {
	addSearchTerm(tweet.getScreenName(), term);
}
*/

// This function is called indirectly from the streaming API, which are moving over
// to polling the REST api, because some search terms weren't coming into the stream
// somehow.					
void ofxWWTweetParticleManager::onNewSearchTerm(TwitterAppEvent& event) {	
	printf("\n\n\nSearch term here!!!\n\n\n");
	addSearchTerm(event.tweet.getScreenName(), event.search_term);
	
}


void ofxWWTweetParticleManager::addSearchTerm(const string& user, const string& term) {
	ofxWWSearchTerm searchTerm;
	searchTerm.pos = ofVec2f(ofRandom(wallRepulsionDistance, simulationWidth-wallRepulsionDistance), 
							 ofRandom(wallRepulsionDistance, simulationHeight-wallRepulsionDistance));
	searchTerm.manager = this;
	searchTerm.term = term;
	searchTerm.user = user;
	
	printf(">>>>>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<\n", searchTerm.term.c_str());
	printf(">>>>>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<\n", searchTerm.term.c_str());
	printf(">>>>>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<\n", searchTerm.term.c_str());
	printf(">>>>>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<\n", searchTerm.term.c_str());
	printf(">>>>>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<\n", searchTerm.term.c_str());
	printf(">>>>>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<\n", searchTerm.term.c_str());
	printf(">>>>>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<\n", searchTerm.term.c_str());
	printf(">>>>>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<\n", searchTerm.term.c_str());
	printf(">>>>>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<\n", searchTerm.term.c_str());
	printf(">>>>>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<\n", searchTerm.term.c_str());

	incomingSearchTerms.push(searchTerm);	
	if(db_provider != NULL) {
		//printf("+++++++++++ updating new particles.\n");
		//setSearchInfoForNewParticles
		//db_provider->setSearchInfoForNewParticles(user, term );
	}

	incomingSearchTerms.push(searchTerm);
	should_take_picture_on = ofGetElapsedTimef()+1.5;

}
/*
void ofxWWTweetParticleManager::onStatusDestroy(const rtt::StatusDestroy& destroy){
}

void ofxWWTweetParticleManager::onStreamEvent(const rtt::StreamEvent& event){
}
*/

// toggle to a new provider.
void ofxWWTweetParticleManager::setCurrentProvider(TweetProvider* prov) {
	if(current_provider != NULL) {
		current_provider->disable();
	}
	current_provider = prov;
	current_provider->enable();
}

TwitterApp& ofxWWTweetParticleManager::getTwitterApp() {
	return twitter;
}


void ofxWWTweetParticleManager::touchUp() {
	selectedSearchTermIndex = -1;
}

