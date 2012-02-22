#include "DefaultForce.h"
#include "ofxWWTweetParticle.h"
#include "ofxWWTweetParticleManager.h"

DefaultForce::DefaultForce(ofxWWTweetParticleManager& manager)
	:Force(manager)
	,lastTimeActivated(-1000)
	,should_hide_on(0)
	,hide_duration_millis(1800) // TODO add to settings
	,hide_force(28.0f) // TODO add to settings
{
	center.set(manager.simulationWidth * 0.5,manager.simulationHeight * 0.5);
}

bool DefaultForce::isReadyWithHiding() {
	return (ofGetElapsedTimeMillis() >= should_hide_on);
}

void DefaultForce::activateParticle(ofxWWTweetParticle& p) {
}

void DefaultForce::deactivateParticle(ofxWWTweetParticle& p) {
	p.static_force = (p.pos - center).normalize() * hide_force;
	p.setState(ofxWWTweetParticle::STATE_HIDING);
}

// called once when showing.
void DefaultForce::activate() {
	lastTimeActivated = ofGetElapsedTimef();
	
	should_hide_on = ofGetElapsedTimeMillis() + hide_duration_millis; // TODO make setting
	vector<ofxWWTweetParticle>::iterator it = tweets.begin();
	while(it != tweets.end()) {
		ofxWWTweetParticle& p = (*it);
		//p.force.y += (manager.tweetFlowSpeed + p.speedAdjust) * (1-p.clampedSelectionWeight);
		++it;
	}
}

// called once, when hiding.
void DefaultForce::deactivate() {
	should_hide_on = ofGetElapsedTimeMillis() + hide_duration_millis; // TODO make setting
	vector<ofxWWTweetParticle>::iterator it = tweets.begin();
	while(it != tweets.end()) {
		ofxWWTweetParticle& p = (*it);
		p.static_force = (p.pos - center).normalize() * hide_force;
		p.setState(ofxWWTweetParticle::STATE_HIDING);
		++it;
	}
}

// called when "active" mode
void DefaultForce::show() {
	size_t len = tweets.size();
	float fadeInUpSpeed = ofMap(ofGetElapsedTimef(), lastTimeActivated, lastTimeActivated+1.5, 0, 1, true);
	for(int i = 0; i < len; i++){
		ofxWWTweetParticle& tweet = tweets[i];
		if(tweet.isSearchTweet){
			continue;
		}
		
		
		tweets[i].force.y += fadeInUpSpeed*(manager.tweetFlowSpeed + tweet.speedAdjust) * (1-tweet.clampedSelectionWeight);
	}
}

void DefaultForce::hide() {
	// repulse from selected search term.
	ofxWWSearchTerm selected_term;
	if(manager.getSearchTermManager().getSelectedSearchTerm(selected_term)) {
		center = selected_term.pos;
	}
	
	float now = ofGetElapsedTimeMillis();
	float diff = should_hide_on - now;
	float p = MAX(0, (diff / hide_duration_millis)); // TODO add to settings

	// repulse from 
	vector<ofxWWTweetParticle>::iterator it = tweets.begin();
	ofVec2f dir;
	float dist = 0;
	float dist_sq = 0;
	float f = 0;
	float rest = 500 +sin(p*HALF_PI)*2000; // TODO add to settings.
	float duration_influance = sin(p*HALF_PI);
	float flow_force = manager.tweetFlowSpeed * 2;
	while(it != tweets.end()) {
		ofxWWTweetParticle& tweet = (*it);
		tweet.dot_opacity = p;
		dir = (center - tweet.pos);
		dist = dir.length();
		f =  (dist-rest) * (0.03 * duration_influance); // TODO add to settings
		dir.normalize();
		dir *= f;
		tweet.force += dir;
		tweet.force.y += (flow_force + tweet.speedAdjust) * (1-tweet.clampedSelectionWeight);
		++it;
	}	
	
	// repulse from eachother.
	it = tweets.begin();
	vector<ofxWWTweetParticle>::iterator other_it ;
	while(it != tweets.end()) {
		other_it = it+1;
		ofxWWTweetParticle& a = *it;
		while(other_it != tweets.end()) {
			ofxWWTweetParticle& b = *other_it;
			dir = (b.pos-a.pos);
			dist_sq = dir.lengthSquared();
			f = 1.0/dist_sq;
			if(f > 0.04) {
				dir *= f * (60 * duration_influance);  // TODO add to settings
				a.force += dir;
				b.force -= dir;
			}
				++other_it;
			
		}
		++it;
	}
}

