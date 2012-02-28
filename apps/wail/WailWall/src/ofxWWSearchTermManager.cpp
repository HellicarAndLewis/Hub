#include "ofxWWSearchTermManager.h"
#include "ofxWWTweetParticleManager.h"


ofxWWSearchTermManager::ofxWWSearchTermManager()
	:screenshot_userdata(NULL)
	,should_take_picture_on(FLT_MAX)
	,selectedSearchTermIndex(-1)
	,tweetSearchMinWaitTime(1)
	,fadeOutTime(1)
	,movement(1)
	,twitter(NULL)
	,deselectionDelay(2)
	,searchTermSelectionRadiusPercent(0.1)
	,searchTermMinAlpha(0.35)
,selectedTermMinAlpha(0.7)
{

}


void ofxWWSearchTermManager::setup(TwitterApp *twitter, ofxWWTweetParticleManager *parent) {
	// for testing
	addSearchLayerListener(this);
	
	this->parent = parent;
	this->twitter = twitter;
}

void ofxWWSearchTermManager::update() {
	
	
	
	
	handleTweetSearch();
	
	
	
	
	doTouchInteraction();

	if(handRemovedTimer.done()) {
		handRemovedTimer.reset();
		// fade out the selected search term, (make sure you check it's in range)
		if(selectedSearchTermIndex>=0 && selectedSearchTermIndex<searchTerms.size()) {
			//searchTerms[selectedSearchTermIndex].fade();
		}
		
		// send an event
		// don't send twice!
		if(lastSearchTermSelectionSentAsEvent!="") {
			lastSearchTermSelectionSentAsEvent = "";
			
			// and fire events
			for(int j = 0; j < listeners.size(); j++) {
				listeners[j]->onAllSearchTermsDeselected();
			}
		}
		selectedSearchTermIndex = -1;
	}
	if(ofGetElapsedTimef() > should_take_picture_on) {
		//screenshot_callback(screenshot_searchterm.user, screenshot_userdata); // roxlu
		should_take_picture_on = FLT_MAX;
		
		// do not store this item in the queue.
		if(twitter != NULL) {
			twitter->setSearchTermAsUsed(
						 screenshot_searchterm.user
						,screenshot_searchterm.term
			);
		}
	}
}

void ofxWWSearchTermManager::doTouchInteraction() {
	
	
	
	doSearchTermSelectionTest();
	
	
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
	float squaredMinDistance = repulsionDistance*repulsionDistance;
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
				ofVec2f force = (awayFromOther * ((repulsionDistance - distance) * repulsionAttenuation));
				searchTerms[i].force += force;			
			}
		}
	}
	
	for(int i = 0; i < searchTerms.size(); i++){
		if(searchTerms[i].dead && ofGetElapsedTimef() - searchTerms[i].killedTime > fadeOutTime){
			
			// if the selectedSearchTermIndex is later in the list than i, decrement it as we're shuffling along one
			if(i<selectedSearchTermIndex) selectedSearchTermIndex--;
			// otherwise, if we're (god forbid) deleting the selected search term, do an unselect.
			else if(i==selectedSearchTermIndex) {
				selectedSearchTermIndex = -1;
				
				// don't send twice!
				if(lastSearchTermSelectionSentAsEvent!="") {
					lastSearchTermSelectionSentAsEvent = "";
				
					// and fire events
					for(int j = 0; j < listeners.size(); j++) {
						listeners[j]->onAllSearchTermsDeselected();
					}
				}
			}
			
			searchTerms.erase(searchTerms.begin()+i);
			i--;
			
		}
	}
	
	// if there's a selected search term and we're on layer 2, 
	// attract the search term to the nearest touch
	if(selectedSearchTermIndex>-1 && parent->tweetLayerOpacity < 0.5) {
		float minDistSqr = FLT_MAX;
		int blobId = -1;
		ofVec2f blobCoord;
		
		ofVec2f termCoord = searchTerms[selectedSearchTermIndex].pos;
		
		map<int, KinectTouch>::iterator it = parent->blobsRef->begin();
		
		while(it != parent->blobsRef->end()) {
			blobCoord = ofVec2f((*it).second.x*parent->simulationWidth, (*it).second.y*parent->simulationHeight);
			
			float distSqr = blobCoord.distanceSquared(termCoord);
			if(distSqr<minDistSqr) {
				minDistSqr = distSqr;
				blobId = (*it).first;
				
			}
			++it;
		}
		
		
		float attractionSpeed = 0.05;
		// we've found a closest blob to the search term, so attract it!!
		if(blobId!=-1) {
			// attract to the blob
			ofVec2f dist = blobCoord - termCoord;
			float len = dist.length();
			if(len<200) {
				searchTerms[selectedSearchTermIndex].pos = termCoord*(1.f-attractionSpeed) + blobCoord*(attractionSpeed);
			} else {// falloff
				attractionSpeed = ofMap(len, 200, 300, attractionSpeed, 0, true);
				searchTerms[selectedSearchTermIndex].pos = termCoord*(1.f-attractionSpeed) + blobCoord*(attractionSpeed);
			}
		}
	}
	//apply a float and a wobble
	
	for(int i = 0; i < searchTerms.size(); i++){
		searchTerms[i].touchPresent = parent->blobsRef->size() != 0;
		//cout << "accumulated force is " << searchTerms[i].force  << endl;
		searchTerms[i].update();
	}
}


string lastSearchTerm;
float lastTime = 0;
void ofxWWSearchTermManager::onSearchTermSelected(const SearchTermSelectionInfo& term) {
	lastSearchTerm = term.term;
	lastTime = ofGetElapsedTimef();

	for(int i = 0; i < searchTerms.size(); i++) {
		if(i==selectedSearchTermIndex) {
			searchTerms[selectedSearchTermIndex].select();
		} else {
			searchTerms[i].deselect();
		}
	}
}
void ofxWWSearchTermManager::onAllSearchTermsDeselected() {
	lastSearchTerm = "";
	lastTime = ofGetElapsedTimef();
	for(int i = 0; i < searchTerms.size(); i++) {
		searchTerms[i].deselect();
	}
}


void ofxWWSearchTermManager::render() {
	for(int i = 0; i < searchTerms.size(); i++){
		searchTerms[i].draw();
	}
	
	/*// debugging selection
	float alpha = ofMap(ofGetElapsedTimef(), lastTime, lastTime + 2,  1.5, 0, true );
	
	if(alpha>0) {
		string msg = "";
		if(lastSearchTerm=="") {
			msg = "Deselected";
		} else {
			msg = "Selected " + lastSearchTerm;
		}
		glColor4f(1, 0, 0, alpha);
		parent->sharedSearchFont.drawString(msg, 1000, 1000);
		
	}*/
	
}
void ofxWWSearchTermManager::addSearchTerm(const string& user, const string& term, bool isUsed) {
	ofxWWSearchTerm search_term;
	search_term.pos = ofVec2f(ofRandom(parent->wallRepulsionDistance, parent->simulationWidth-parent->wallRepulsionDistance), 
							 ofRandom(parent->wallRepulsionDistance, parent->simulationHeight-parent->wallRepulsionDistance));
	search_term.manager = this;
	search_term.term = term;
	search_term.user = user;
	search_term.took_screenshot = isUsed;
	if(!isUsed) {
		printf(">>>>>>>>>>>>>>>>>>>>>>>> %s <<<<<<<<<<<<<<<<<<<<<<<<<<\n", search_term.term.c_str());
		incomingSearchTerms.push(search_term);
	}
	else {
		// When the search term is already used we directly add it to the queue.
		// This part is only executed when when we start the applications and 
		// retrieve the mentions from twitter.
		searchTerms.push_back(search_term);
	}

}


void ofxWWSearchTermManager::doSearchTermSelectionTest() {
	int len = searchTerms.size();
	
	// Check if there is a hand in the plinth.
	if(parent->blobsRef->empty()) {
		//printf("(1)\n");
		for(int i = 0; i < len; ++i) {
			
			// don't necessarily fade out the actual touched one
			// as the hand might be out temporarily.
			if(i!=selectedSearchTermIndex) {
//				searchTerms[i].fade();
			}
			
		}
		
		return;
	}
	
	
	int closest_search_term_index = -1;
	float smallest_dist_sq = FLT_MAX;
	float in_range_dist = searchTermSelectionRadiusPercent * parent->simulationWidth;
	in_range_dist *= in_range_dist;
	//printf(">> %f\n", in_range_dist);
	
	for(int i = 0; i < len; ++i) {
		ofxWWSearchTerm& search_term = searchTerms.at(i);
		
		map<int, KinectTouch>::iterator kinect_iter = parent->blobsRef->begin();
		while(kinect_iter != parent->blobsRef->end()) {
			KinectTouch& touch = (kinect_iter->second);
			ofVec2f kinect_pos(touch.x * parent->simulationWidth, touch.y * parent->simulationHeight);
			
			// check if current search term is closer then then once handled so far.
			float dist_sq = kinect_pos.distanceSquared(search_term.pos);
			if(dist_sq < smallest_dist_sq && dist_sq <= in_range_dist) {
				smallest_dist_sq = dist_sq;
				closest_search_term_index = i;	
			}
			
			++kinect_iter;
		}
	}
	
	
	//printf("Smallest dist: %f - tweetLayerOpacity: %f\n", smallest_dist_sq, parent->tweetLayerOpacity);
	
	if(parent->tweetLayerOpacity >= 0.5) {
		//printf("(2,2,2,2,2,2,2,2,2,2,2	)\n");
		if(selectedSearchTermIndex>=0 && selectedSearchTermIndex<searchTerms.size()) {
	//		searchTerms[selectedSearchTermIndex].highlight();
		}

		return;
	}
	else {
		
		if(closest_search_term_index == -1) {
			
			// begin the timer for deselection if
			// not in range of a search term.
			if(!handRemovedTimer.running()) handRemovedTimer.start(deselectionDelay);

		}
		else {

			// in range of a search term
			ofxWWSearchTerm& selected_term = searchTerms[closest_search_term_index];			
			// cleanup counters.
			for(int i = 0; i < len; ++i) {
				if(i == closest_search_term_index) {
					continue;
				}
//				searchTerms[i].fade();
				searchTerms[i].selection_started_on = 0;
			}
			
			
			
			
			// start counter for selected
			if(selected_term.selection_started_on > 0)  {
				float now = ofGetElapsedTimeMillis();
				float selection_activate_on = selected_term.selection_started_on+1000;
				if(now > selection_activate_on) {
					// TODO add listener/event here
					setSelectedSearchTerm(selected_term);
				}
			}
			else {
				selected_term.selection_started_on = ofGetElapsedTimeMillis();
				selected_term.warmUp();
			}
			
			
			
			
			
			
		}
	}
	//printf("Found search term index: %d\n", closest_search_term_index);
}



void ofxWWSearchTermManager::setSelectedSearchTerm(ofxWWSearchTerm &searchTerm) {
	
	
	// first of all, check the searchterm exists in our array.
	for(int i = 0; i < searchTerms.size(); i++) {
		if(searchTerms[i].term==searchTerm.term && searchTerms[i].user == searchTerm.user) {
			
			// set the selected search term
			selectedSearchTermIndex = i;
			
			// make it highlighted
//			searchTerm.highlight();
			
			// send an event
			if(lastSearchTermSelectionSentAsEvent!=searchTerm.term) {
				
				lastSearchTermSelectionSentAsEvent = searchTerm.term;
				SearchTermSelectionInfo info;
				info.term = searchTerm.term;
				info.position = searchTerm.pos;
				
				for(int j = 0; j < listeners.size(); j++) {
					listeners[j]->onSearchTermSelected(info);
				}
			}
			return;
		}
	}
	printf("Error! Can't find the search term '%s' in the list!\n", searchTerm.term.c_str());
	
}

bool ofxWWSearchTermManager::getSelectedSearchTerm(ofxWWSearchTerm& result) {
	if(selectedSearchTermIndex == -1) {
		return false;
	}
	if(selectedSearchTermIndex >= searchTerms.size()) {
		return false;
	}	
	result = searchTerms[selectedSearchTermIndex];
	return true;
}

void ofxWWSearchTermManager::touchUp() {
//	selectedSearchTermIndex = -1;
	handRemovedTimer.start(deselectionDelay);
}

void ofxWWSearchTermManager::touchDown() {
	handRemovedTimer.reset();
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
		searchTerms.push_back(term);
		
		// as soon as the term gets popped from the queue and added to the array 
		// which gets rendered we tell the system to make a screenshot after
		// X-seconds
		should_take_picture_on = ofGetElapsedTimef()+1.5;
		screenshot_searchterm = term; 
		
		if(searchTerms.size() > maxSearchTerms){
			searchTerms[0].dead = true;
			searchTerms[0].killedTime = ofGetElapsedTimef();
		}
	}
	
}


void ofxWWSearchTermManager::addSearchLayerListener(SearchLayerListener *listener) {
	listeners.push_back(listener);
}

void ofxWWSearchTermManager::setScreenshotCallback(takeScreenshotCallback func, void* userdata) {
	screenshot_callback = func;
	screenshot_userdata = userdata;
}


bool ofxWWSearchTermManager::getSearchTermForWhichWeNeedToTakeScreenshot(ofxWWSearchTerm& term) {
	for(int i = 0; i < searchTerms.size(); ++i) {
		if(!searchTerms[i].took_screenshot) {
			term = searchTerms[i];
			return true;
		}		
	}
	return false;
}

void ofxWWSearchTermManager::selectSearchTerm(const string& term) {
	for(int i = 0; i < searchTerms.size(); ++i) {
		if(searchTerms[i].term == term) {
			searchTerms[i].select();
			return ;
		}		
	}
}

void ofxWWSearchTermManager::deselectSearchTerm(const string& term) {
	for(int i = 0; i < searchTerms.size(); ++i) {
		if(searchTerms[i].term == term) {
			searchTerms[i].deselect();
			return ;
		}		
	}
}

bool ofxWWSearchTermManager::setTookScreenshotForSearchTerm(ofxWWSearchTerm& term) {
	vector<ofxWWSearchTerm>::iterator it = searchTerms.begin();
	while(it != searchTerms.end()) {
	
		// skip terms for which we already created screenshots.
		ofxWWSearchTerm& st = (*it);
		if(st.took_screenshot) {
			++it;
			continue;
		}
		
		// same searchterm?
		if(st.user == term.user && st.term == term.term) {
			st.took_screenshot = true;
			return true;
		}
		++it;
	};
	return false;

}
