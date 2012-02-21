//
//  ofxWWSearchTermManager.cpp
//  WailWall
//
//  Created by Joel Lewis on 20/02/2012.
//  Copyright (c) 2012 Hellicar & Lewis. All rights reserved.
//

#include "ofxWWSearchTermManager.h"
#include "ofxWWTweetParticleManager.h"

ofxWWSearchTermManager::ofxWWSearchTermManager() {
	selectedSearchTermIndex = -1;
	
}


void ofxWWSearchTermManager::setup(TwitterApp *twitter, ofxWWTweetParticleManager *parent) {
	this->parent = parent;
	this->twitter = twitter;
	// Get previously received search terms.
	// -------------------------------------
	vector<TwitterSearchTerm*> stored_search_terms;
	if(twitter->getUnusedSearchTerms(stored_search_terms)) {
		vector<TwitterSearchTerm*>::iterator it = stored_search_terms.begin();
		while(it != stored_search_terms.end()) {
			addSearchTerm((*it)->user, (*it)->search_term);
			++it;
		}
	}
}
void ofxWWSearchTermManager::update() {
	
	if(parent->blobsRef->size() > 0){
		handleTouchSearch();
	}
	
	
	
	handleTweetSearch();
	
	
	
	
	doTouchInteraction();
}

void ofxWWSearchTermManager::doTouchInteraction() {
	
	
	
	int closestSearchTerm = -1;
	float closestDistanceSq = FLT_MAX;
	
	//find the closest touch
	if(!parent->blobsRef->empty()){
		for(int i = 0; i < searchTerms.size(); i++){
			searchTerms[i].closestDistanceSquared = 9999999;
			map<int,KinectTouch>::iterator it;
			for(it = parent->blobsRef->begin(); it != parent->blobsRef->end(); it++){
				ofVec2f point = ofVec2f(it->second.x*parent->simulationWidth, it->second.y*parent->simulationHeight);
				float squareDistance = point.distanceSquared(searchTerms[i].pos);
				if(squareDistance < searchTerms[i].closestDistanceSquared){
					searchTerms[i].closestDistanceSquared = squareDistance;
					searchTerms[i].closestPoint = point;
					searchTerms[i].closestTouchID = it->first;
				}
				
				
				//printf("tweet %f\n", tweetLayerOpacity);
				// we can choose a selectedSearchTermIndex here
				if(parent->tweetLayerOpacity<=0.2) {
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
	if(parent->tweetLayerOpacity<=0.2) {
		
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
		if (searchTerms[i].pos.x < parent->wallRepulsionDistance) {
			searchTerms[i].force.x += (parent->wallRepulsionDistance - searchTerms[i].pos.x) * parent->wallRepulsionAtten;
			searchTerms[i].wallForceApplied = true;
		}
		//RIGHT WALL
		if ((searchTerms[i].pos.x) > (parent->simulationWidth-parent->wallRepulsionDistance)) {
			searchTerms[i].force.x += ( (parent->simulationWidth-parent->wallRepulsionDistance) - (searchTerms[i].pos.x) ) * parent->wallRepulsionAtten;
			searchTerms[i].wallForceApplied = true;
		}
		//TOP
		if (searchTerms[i].pos.y < parent->wallRepulsionDistance) {
			searchTerms[i].force.y += (parent->wallRepulsionDistance - searchTerms[i].pos.y) * parent->wallRepulsionAtten;
			searchTerms[i].wallForceApplied = true;
		}		
		//BOTTOM
		if ((searchTerms[i].pos.y) > (parent->simulationHeight-parent->wallRepulsionDistance)) {
			searchTerms[i].force.y += ( (parent->simulationHeight-parent->wallRepulsionDistance)  - searchTerms[i].pos.y) * parent->wallRepulsionAtten;
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
		searchTerms[i].touchPresent = parent->blobsRef->size() != 0;
		//cout << "accumulated force is " << searchTerms[i].force  << endl;
		searchTerms[i].update();
	}
}

void ofxWWSearchTermManager::render() {
	for(int i = 0; i < searchTerms.size(); i++){
		searchTerms[i].draw();
	}
}
void ofxWWSearchTermManager::addSearchTerm(const string& user, const string& term) {
	ofxWWSearchTerm searchTerm;
	searchTerm.pos = ofVec2f(ofRandom(parent->wallRepulsionDistance, parent->simulationWidth-parent->wallRepulsionDistance), 
							 ofRandom(parent->wallRepulsionDistance, parent->simulationHeight-parent->wallRepulsionDistance));
	searchTerm.manager = this;
	searchTerm.term = term;
	searchTerm.user = user;
	printf(">>>>>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<\n", searchTerm.term.c_str());
	incomingSearchTerms.push(searchTerm);
}




void ofxWWSearchTermManager::deselectAllSearchTerms() {
	selectedSearchTermIndex = -1;
}

void ofxWWSearchTermManager::handleTouchSearch() {
	
	bool searchDebug = false;
	if(searchDebug) cout << "++++++ SEARCH DEBUG QUERY " << endl;
	
	int oldSelectedSearchTermIndex = selectedSearchTermIndex;
	selectedSearchTermIndex = -1;
	
	//look for a selected search term
	for(int i = 0; i < searchTerms.size(); i++){
		
		if(searchTerms[i].selected){
			
			selectedSearchTermIndex = i;
			parent->shouldTriggerScreenshot = false;
			break;
		}
	}
	
	if(oldSelectedSearchTermIndex!=selectedSearchTermIndex) {
		if(selectedSearchTermIndex==-1) {
			parent->setCurrentProvider(parent->stream_provider);
		} else {
			parent->db_provider->fillWithTweetsWhichContainTerm(searchTerms[selectedSearchTermIndex].term);
			parent->setCurrentProvider(parent->db_provider);
		}
	}
	
}

void ofxWWSearchTermManager::handleTweetSearch(){
	
	
	// don't allow this to happen too often
	if(ofGetElapsedTimef() - lastSearchTermTime < tweetSearchMinWaitTime){
		return;
	}
	
	lastSearchTermTime = ofGetElapsedTimef();
	
	if(!incomingSearchTerms.empty()){
		ofxWWSearchTerm term = incomingSearchTerms.front();
		incomingSearchTerms.pop();
		
		parent->shouldTriggerScreenshot = true;
		//selectedSearchTermIndex = searchTerms.size();
		searchTerms.push_back(term);
		
		if(searchTerms.size() > maxSearchTerms){
			searchTerms[0].dead = true;
			searchTerms[0].killedTime = ofGetElapsedTimef();
		}
	}
	
}
