#include "ofxWWRenderer.h"
#include "ofxWWTweetParticleManager.h"
#include "ofxWebSimpleGuiToo.h"
#include "Error.h"

ofxWWTweetParticleManager::ofxWWTweetParticleManager():

	
	renderer(NULL)
	,screenshot_userdata(NULL)
	,current_provider(NULL)
	,stream_provider(NULL)
	,db_provider(NULL)
{
	maxTweets = 100;

	
	callToActionTime = 5;
	should_take_picture_on = FLT_MAX;
}

void ofxWWTweetParticleManager::setup(ofxWWRenderer* ren){
	renderer = ren;
	
	// Initialize twitter.	
	// -------------------
	twitter.init(4444);
	
	if(!twitter.connect()) {
		printf("Error: cannot connect to twitter stream.\n");
	}
	// 
	twitter.addNewSearchTermListener(this, &ofxWWTweetParticleManager::onNewSearchTerm);

	searchTerms.setup(&twitter, this);
	
	burstOne.loadImage("images/burst1.png");
	burstTwo.loadImage("images/burst2.png");
	
	setupColors();
	
	// Create tweet providers
	// ----------------------
	stream_provider = new TweetProviderStream(twitter);
	db_provider = new TweetProviderDB(twitter);
	stream_provider->addListener(this);
	db_provider->addListener(this);
	setCurrentProvider(stream_provider);
	twitter.addCustomStreamListener(*stream_provider); // stream provider wants to listen to incoming tweets.
	
	ofAddListener(ofEvents.keyPressed, this, &ofxWWTweetParticleManager::keyPressed);
}

void ofxWWTweetParticleManager::setScreenshotCallback(takeScreenshotCallback func, void* userdata) {
	screenshot_callback = func;
	screenshot_userdata = userdata;
}

void ofxWWTweetParticleManager::keyPressed(ofKeyEventArgs& args) {
	/*if(args.key == '!'){
		int size = fakeSearchTerms.size();
		int dx = ofRandom(size);	
		string term = fakeSearchTerms.at(dx);
		printf("Using fake search term: %s\n", term.c_str());		
		addSearchTerm("no_user", term );
	}
	else */if(args.key == '3') {
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
	webGui.addSlider("Max Search Terms", searchTerms.maxSearchTerms, 5, 15);
	webGui.addSlider("Search Font Size", searchTerms.searchTermFontSize, 100, 500);
	webGui.addSlider("Search Min Opacity", searchTerms.searchMinOpacity, 0, .4);
	webGui.addSlider("Touch Min Dist", searchTerms.searchTermMinDistance, 50, 1000);
	webGui.addSlider("Touch Min Hold", searchTerms.searchTermMinHoldTime, .5, 3.0);
	
	// do we need these anymore?
	
	
	webGui.addSlider("Call to action time", callToActionTime, 5, 60);
	
	
	
	webGui.addToggle("Search Debug", searchTerms.drawSearchDebug);
	
	webGui.addPage("Search Term Animation");
	webGui.addSlider("Wall Repulsion Dist", wallRepulsionDistance, 0, 900);
	webGui.addSlider("Search Repulse Dist", searchTerms.searchTermRepulsionDistance, 500, 2000);
	webGui.addSlider("Search Repulse Atten", searchTerms.searchTermRepulsionAttenuation, 0, .2);
	webGui.addSlider("Search Hand Attract", searchTerms.searchTermHandAttractionFactor, 0, .1);
	
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
	searchTerms.update();
	
	updateTweets();
		
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
		
		if(!sharedSearchFont.isLoaded() || searchTerms.searchTermFontSize != sharedSearchFont.getSize()){
			if(!sharedSearchFont.loadFont("fonts/montreal-ttf/Montreal-BoldIta.ttf", searchTerms.searchTermFontSize, true, true, false)){
				ofLogError("ofxWWTweetParticleManager::setup() -- couldn't load search  font!");
			}	
			cout << "search font allocating! " << searchTerms.searchTermFontSize << " " << sharedSearchFont.getSize() << endl;
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





void ofxWWTweetParticleManager::addCurrentRenderToScreenshotQueue() {
	if(screenshot_userdata == NULL) {
		return;
	}
	// TODO: add correct username
	//screenshot_callback("joelgethinlewis", screenshot_userdata);
}

float ofxWWTweetParticleManager::weightBetweenPoints(ofVec2f touch, float normalizedSize, ofVec2f tweet){
	float touchMid = normalizedSize*simulationHeight*touchSizeScale;
	return MAX( ofMap(touch.distance(tweet), touchMid-touchInfluenceFalloff/2., touchMid+touchInfluenceFalloff/2., 1.0, 0.0, false), 0.0);
}

void ofxWWTweetParticleManager::updateTweets(){
	
	for(int i = tweets.size()-1; i >= 0; i--){

		
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
		//TOP
		if (tweets[i].pos.y < 0) {
			tweets[i].force.y += (-tweets[i].pos.y) * wallRepulsionAtten;
		}
		
		//BOTTOM
		if ((tweets[i].pos.y) > (simulationHeight)) {
			tweets[i].force.y += ( (simulationHeight)  - (tweets[i].pos.y)) * wallRepulsionAtten;
		}
	}
		
	//apply flow
	for(int i = 0; i < tweets.size(); i++){
		if(tweets[i].isSearchTweet){
			continue;
		}
		ofVec2f forceVector(0,0);
		forceVector.y += (tweetFlowSpeed + tweets[i].speedAdjust) * (1-tweets[i].clampedSelectionWeight);
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


void ofxWWTweetParticleManager::doSearchTermSelectionTest() {
	int len = searchTerms.size();
	if(blobsRef->empty()) {
		printf("(1)\n");
		for(int i = 0; i < len; ++i) {
			searchTerms[i].fade();
		}
		return;
	}
	
	
	int closest_search_term_index = -1;
	float smallest_dist_sq = FLT_MAX;
	float in_range_dist = 0.1 * simulationWidth;
	in_range_dist *= in_range_dist;
	printf(">> %f\n", in_range_dist);
	
	for(int i = 0; i < len; ++i) {
		ofxWWSearchTerm& search_term = searchTerms.at(i);
		
		map<int, KinectTouch>::iterator kinect_iter = blobsRef->begin();
		while(kinect_iter != blobsRef->end()) {
			KinectTouch& touch = (kinect_iter->second);
			ofVec2f kinect_pos(touch.x * simulationWidth, touch.y * simulationHeight);
			
			// check if current search term is closer then then once handled so far.
			float dist_sq = kinect_pos.distanceSquared(search_term.pos);
			if(dist_sq < smallest_dist_sq && dist_sq <= in_range_dist) {
				smallest_dist_sq = dist_sq;
				closest_search_term_index = i;	
			}
			
			++kinect_iter;
		}
	}
	
	
	printf("Smallest dist: %f - tweetLayerOpacity: %f\n", smallest_dist_sq, tweetLayerOpacity);
	
	if(tweetLayerOpacity >= 0.5) {
		printf("(2,2,2,2,2,2,2,2,2,2,2	)\n");
		return;
	}
	else {
		
		if(closest_search_term_index == -1) {
			// not in range of a search term.
			
		}
		else {
		
			// in range of a search term
			ofxWWSearchTerm& selected_term = searchTerms[closest_search_term_index];
			
			// cleanup counters.
			for(int i = 0; i < len; ++i) {
				if(i == closest_search_term_index) {
					continue;
				}
				searchTerms[i].fade();
				searchTerms[i].selection_started_on = 0;
			}
			
			// start counter for selected
			if(selected_term.selection_started_on > 0)  {
				float now = ofGetElapsedTimeMillis();
				float selection_activate_on = selected_term.selection_started_on+1000;
				if(now > selection_activate_on) {
					selected_term.highlight();
				}
			}
			else {
				selected_term.selection_started_on = ofGetElapsedTimeMillis();
			}
			
		}
	}
	printf("Found search term index: %d\n", closest_search_term_index);
}

void ofxWWTweetParticleManager::updateSearchTerms(){
		
	if(clearTweets){
		tweets.clear();
		clearTweets = false;
	}
	
	
	int closestSearchTerm = -1;
	float closestDistanceSq = FLT_MAX;
	
	doSearchTermSelectionTest();
	
	//find the closest touch
/*
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
*/	
	
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
	searchTerms.render();
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
	
//	if(isDoingSearch){
//		for(int i = 0; i < tweets.size(); i++){
//			if(tweets[i].isSearchTweet){
////				cout << "++++++ DRAWING CAUSTICS BETWEEN " << tweets[i].pos << " " << searchTerms[selectedSearchTerm].pos << endl;
//				attemptConnection(tweets[i].pos, 1.0, searchTerms[selectedSearchTermIndex].pos, 1.0, 1-tweetLayerOpacity);
//			}
//		}
//	}
	
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
	//printf(">> [ok] : %s\n", tweet.getText().c_str());	
	ofxWWTweetParticle particle = createParticleForTweet(tweet);
	tweets.push_back(particle);
}



ofxWWTweetParticle ofxWWTweetParticleManager::createParticleForTweet(const rtt::Tweet& tweet){
	ofxWWTweetParticle tweetParticle;
	tweetParticle.manager = this;
	if(tweetFlowSpeed != 0){
	
	
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
					
void ofxWWTweetParticleManager::onNewSearchTerm(TwitterAppEvent& event) {
	// send search term to its manager
	searchTerms.addSearchTerm(event.tweet.getScreenName(), event.search_term);
	
	// prepare to take a screenshot
	should_take_picture_on = ofGetElapsedTimef()+1.5;
	
	// notify provider
	db_provider->setSearchInfoForNewParticles(event.tweet.getScreenName(), event.search_term);
}



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
	searchTerms.deselectAllSearchTerms();
}

