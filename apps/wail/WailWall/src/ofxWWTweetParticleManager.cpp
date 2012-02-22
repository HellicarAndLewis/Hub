#include "ofxWWRenderer.h"
#include "ofxWWTweetParticleManager.h"
#include "ofxWebSimpleGuiToo.h"
#include "Error.h"
#include "Colours.h"

ofxWWTweetParticleManager::ofxWWTweetParticleManager():
	renderer(NULL)
	,current_provider(NULL)
	,stream_provider(NULL)
	,db_provider(NULL)
	,current_force(NULL)
	,default_force(NULL)
	,selected_force(NULL)
{
	maxTweets = 100;

	Colours::set(AT_SIGN, 0xf6b626);
	Colours::set(LAYER_1_FONT, 0xFFFFFF);
	Colours::set(LAYER_2_FONT, 0xFFFFFF);
	
	callToActionTime = 5;
	//should_take_picture_on = FLT_MAX;
}

void ofxWWTweetParticleManager::setup(ofxWWRenderer* ren){
	renderer = ren;
	
	// Initialize twitter.	
	// -------------------
	twitter.init(4444);
	
	if(!twitter.connect()) {
		printf("Error: cannot connect to twitter stream.\n");
	}
	
	twitter.addNewSearchTermListener(this, &ofxWWTweetParticleManager::onNewSearchTerm);

	searchTermManager.setup(&twitter, this);
	searchTermManager.addSearchLayerListener(this);
	
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
	
	// Forces
	default_force = new DefaultForce(*this);
	selected_force = new SelectedForce(*this);
	setCurrentForce(default_force);
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
	else if(args.key == '5') {
		setCurrentForce(default_force);
		setCurrentProvider(stream_provider);
	}
	else if(args.key == '6') {
		db_provider->fillWithTweetsWhichContainTerm("love");
		setCurrentForce(selected_force);
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
	
					   
	webGui.addPage("Search Term Timing");
	webGui.addSlider("Max Search Terms", searchTermManager.maxSearchTerms, 5, 15);
	webGui.addSlider("tweet Search Min Wait time", searchTermManager.tweetSearchMinWaitTime, 1, 20);
	webGui.addSlider("Search Font Size", searchTermManager.searchTermFontSize, 100, 500);
	webGui.addSlider("Unselected Term Min Alpha", searchTermManager.searchTermMinAlpha, 0.1, 0.9);
	webGui.addSlider("Selected Term Min Alpha", searchTermManager.selectedTermMinAlpha, 0.1, 1);

	
	// do we need these anymore?
	
	
	webGui.addSlider("Call to action time", callToActionTime, 5, 60);
	
	
	
	webGui.addToggle("Search Debug", searchTermManager.drawSearchDebug);
	
	webGui.addPage("Search Term Animation");
	webGui.addSlider("Wall Repulsion Dist", wallRepulsionDistance, 0, 900);
	webGui.addSlider("Search Repulse Dist", searchTermManager.repulsionDistance, 500, 2000);
	webGui.addSlider("Search Repulse Atten", searchTermManager.repulsionAttenuation, 0, .2);
	webGui.addSlider("Search Fadeout Time", searchTermManager.fadeOutTime, 0, 2);
	webGui.addSlider("Search Deselection Delay", searchTermManager.deselectionDelay, 0, 4);
	webGui.addSlider("searchTermSelectionRadiusPercent", searchTermManager.searchTermSelectionRadiusPercent, 0.01, 0.14);
	
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
	searchTermManager.update();
	
	updateTweets();
		
	
}

void ofxWWTweetParticleManager::checkFonts(){
	#ifdef USE_FTGL
	
		if(!sharedTweetFont.isLoaded() || tweetFontSize != sharedTweetFont.getSize()){
			if(!sharedTweetFont.loadFont("fonts/montreal-ttf/Montreal-LightIta.ttf", tweetFontSize, true, true, false)){
				ofLogError("ofxWWTweetParticleManager::setup() -- couldn't load tweet font!");
			}	
			cout << "tweet font allocating! " << tweetFontSize << " " << sharedTweetFont.getSize() << endl;
		}
		
		if(!sharedSearchFont.isLoaded() || searchTermManager.searchTermFontSize != sharedSearchFont.getSize()){
			if(!sharedSearchFont.loadFont("fonts/montreal-ttf/Montreal-BoldIta.ttf", searchTermManager.searchTermFontSize, true, true, false)){
				ofLogError("ofxWWTweetParticleManager::setup() -- couldn't load search  font!");
			}	
			cout << "search font allocating! " << searchTermManager.searchTermFontSize << " " << sharedSearchFont.getSize() << endl;
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


float ofxWWTweetParticleManager::weightBetweenPoints(ofVec2f touch, float normalizedSize, ofVec2f tweet){
	float touchMid = normalizedSize*simulationHeight*touchSizeScale;
	return MAX( ofMap(touch.distance(tweet), touchMid-touchInfluenceFalloff/2., touchMid+touchInfluenceFalloff/2., 1.0, 0.0, false), 0.0);
}

void ofxWWTweetParticleManager::updateTweets(){
	size_t num_tweets = tweets.size();
	
//	printf("Tweets.siz: %zu, flowspeed: %f, wallRepulsionDistance: %f\n"
//			,tweets.size()
//			,tweetFlowSpeed
//			,wallRepulsionDistance
//	);
	
	// remove particles (splitting up for performance).
	if(tweetFlowSpeed <= 0) {
		// tweets going up; 
		vector<ofxWWTweetParticle>::iterator it = tweets.begin();
		while(it != tweets.end()) {
			ofxWWTweetParticle& tweet = *it;
			if(tweet.pos.y < -wallRepulsionDistance) {
				it = tweets.erase(it);
				continue;
			}
			++it;
		}
	}
	else if(tweetFlowSpeed >= 0) {
		// tweets going down;
		vector<ofxWWTweetParticle>::iterator it = tweets.begin();
		float remove_pos = simulationHeight + wallRepulsionDistance;
		while(it != tweets.end()){
			ofxWWTweetParticle& tweet = *it;
			if(tweet.pos.y > remove_pos) {				
				it = tweets.erase(it);
				continue;
			}
			++it;
		} 
	}
	
	// just remove tweets when there are too many 
	{
		if(num_tweets > maxTweets) {
			vector<ofxWWTweetParticle>::iterator it = tweets.begin();
			while(it != tweets.end()) {
				if(!(*it).isDrawingText()) {
					tweets.erase(it);
					break;
				}
				it++;
			}
		}
	}
	
//	for(int i = tweets.size()-1; i >= 0; i--) {
//	
//		
//		if((tweetFlowSpeed <= 0 && tweets[i].pos.y < -wallRepulsionDistance) || 
//		   (tweetFlowSpeed >= 0 && tweets[i].pos.y > simulationHeight+wallRepulsionDistance) )
//		{
//			if(tweets.size() > maxTweets){
//				tweets.erase(tweets.begin()+i);
//			}
//			else{
//
//				//wrap around
//				//tweets[i].pos.y = tweetFlowSpeed > 0 ? -wallRepulsionDistance : simulationHeight + wallRepulsionDistance;
//			}
//		}
//		
//	}
//	
	
	
	{
		float scale = simulationHeight*touchSizeScale+touchInfluenceFalloff;
		ofVec2f touchpoint(0,0);
		map<int,KinectTouch>::iterator kinect_it;
		vector<ofxWWTweetParticle>::iterator tweet_it = tweets.begin();
		
		while(tweet_it != tweets.end()) {
			ofxWWTweetParticle& tweet = *tweet_it;
			tweet.selectionWeight = 0;
			kinect_it = blobsRef->begin();
			
			while(kinect_it != blobsRef->end()) {
				KinectTouch& touch = kinect_it->second;
				float maxRadiusSquared = powf(touch.z * scale * 0.5, 2.0f);	
				touchpoint.set(touch.x*simulationWidth, touch.y*simulationHeight);
				if(touchpoint.distanceSquared(tweet.pos) < maxRadiusSquared) {
					float weightBetween = weightBetweenPoints(touchpoint, touch.size, tweet.pos);
					tweet.selectionWeight += weightBetween;
				}			
				++kinect_it;
			}
			
			(*tweet_it).clampedSelectionWeight = MIN(tweet.selectionWeight, 1.0);
			++tweet_it;
		}
	}
	
	//hand reveal top level tweets
//	map<int,KinectTouch>::iterator it;
//	for(int i = 0; i < tweets.size(); i++){
//		tweets[i].selectionWeight = 0;
//		
//		
//		for(it = blobsRef->begin(); it != blobsRef->end(); it++) {
//			float maxRadiusSquared = powf(it->second.z * simulationHeight*touchSizeScale+touchInfluenceFalloff/2, 2.0f);
//			ofVec2f touchpoint = ofVec2f(it->second.x*simulationWidth, it->second.y*simulationHeight);
//			if(touchpoint.distanceSquared(tweets[i].pos) < maxRadiusSquared){			
//				float weightBetween = weightBetweenPoints(touchpoint, it->second.size, tweets[i].pos);
//				tweets[i].selectionWeight += weightBetween;
//			}
//		}
//		tweets[i].clampedSelectionWeight = MIN(tweets[i].selectionWeight, 1.0);
//	}
	
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
	if(current_force->isHiding()) {
		current_force->hide();
		if(current_force->isReadyWithHiding()){	
			current_force->setShouldHide(false);
			current_force = new_force;
			printf("CHANGED FORCE\n");
		}		
	}
	else {
		current_force->show();
	}
	
	/* 
	for(int i = 0; i < tweets.size(); i++){
		if(tweets[i].isSearchTweet){
			continue;
		}
		ofVec2f forceVector(0,0);
		forceVector.y += (tweetFlowSpeed + tweets[i].speedAdjust) * (1-tweets[i].clampedSelectionWeight);
		tweets[i].force += forceVector;
	}
	*/
	
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




void ofxWWTweetParticleManager::renderTweets(){
	
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	for(int i = 0; i < tweets.size(); i++){
		if(!tweets[i].isSearchTweet){
			tweets[i].drawText();
			tweets[i].drawDot();
			if(drawTweetDebug){
				tweets[i].drawDebug();
			}
		}
	}
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	 
}

void ofxWWTweetParticleManager::renderSearchTerms(){	
	searchTermManager.render();
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
	/*ofxXmlSettings colors;
	if(colors.loadFile(ofToDataPath("fonts/fontcolor.xml"))){
		atSignColor = ofColor::fromHex( ofHexToInt( colors.getValue("colors:atsign", "0xFFFFFF")) );
		layerOneFontColor = ofColor::fromHex( ofHexToInt( colors.getValue("colors:layerone", "0xFFFFFF")) );
		layerTwoFontColor = ofColor::fromHex( ofHexToInt( colors.getValue("colors:layerone", "0xFFFFFF")) );
	}
	else{
		ofSystemAlertDialog("fonts/fontcolor.xml not found!");
	}*/
	
}

void ofxWWTweetParticleManager::onNewTweet(const rtt::Tweet& tweet) {
	//printf(">> [ok] : %s\n", tweet.getText().c_str());	
	ofxWWTweetParticle particle = createParticleForTweet(tweet);
	
	if(current_force->isHiding()) {
		current_force->deactivateParticle(particle);
	}
	else {
		current_force->activateParticle(particle);
	}
	
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
	searchTermManager.addSearchTerm(event.tweet.getScreenName(), event.search_term, event.is_used);
	
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

// toggle forces
void ofxWWTweetParticleManager::setCurrentForce(Force* force) {
	printf("change forces ______________________\n");
	new_force = force;
	if(current_force != NULL) {
		current_force->deactivate();
		current_force->setShouldHide(true);
		new_force->activate();

	}
	else {
		current_force = force;
	}
	/*
	if(current_force != NULL) {
		current_force->deactivate();
	}
	current_force = force;
	current_force->activate();
	*/
	
}

TwitterApp& ofxWWTweetParticleManager::getTwitterApp() {
	return twitter;
}


void ofxWWTweetParticleManager::touchUp() {
	searchTermManager.touchUp();
}

void ofxWWTweetParticleManager::touchDown() {
	searchTermManager.touchDown();
}


void ofxWWTweetParticleManager::onSearchTermSelected(const SearchTermSelectionInfo& term) {
	printf("::::::::::::::::::::::::::::::::::::: %s\n", term.term.c_str());
	db_provider->fillWithTweetsWhichContainTerm(term.term);
	setCurrentProvider(db_provider);
	setCurrentForce(selected_force);
}

void ofxWWTweetParticleManager::onAllSearchTermsDeselected() {
	setCurrentProvider(stream_provider);
	setCurrentForce(default_force);
}
