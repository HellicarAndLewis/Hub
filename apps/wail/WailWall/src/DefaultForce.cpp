#include "DefaultForce.h"
#include "ofxWWTweetParticle.h"
#include "ofxWWTweetParticleManager.h"

DefaultForce::DefaultForce(ofxWWTweetParticleManager& manager)
	:Force(manager)
	,should_hide_on(0)
	,hide_duration_millis(800)
	,hide_force(58.0f)
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
}

void DefaultForce::activate() {

}

// called once, when hiding.
void DefaultForce::deactivate() {
	should_hide_on = ofGetElapsedTimeMillis() + hide_duration_millis; // TODO make setting
	vector<ofxWWTweetParticle>::iterator it = tweets.begin();
	while(it != tweets.end()) {
		ofxWWTweetParticle& p = (*it);
		p.static_force = (p.pos - center).normalize() * hide_force;
		++it;
	}
}

// called when "active" mode
void DefaultForce::show() {
	size_t len = tweets.size();
	for(int i = 0; i < len; i++){
		ofxWWTweetParticle& tweet = tweets[i];
		if(tweet.isSearchTweet){
			continue;
		}
		
		tweets[i].force.y += (manager.tweetFlowSpeed + tweet.speedAdjust) * (1-tweet.clampedSelectionWeight);
	}
}

void DefaultForce::hide() {
	float now = ofGetElapsedTimeMillis();
	float diff = should_hide_on - now;
	float p = MAX(0, (diff / hide_duration_millis));

	// TODO we can use one alpha for this.. instead of updating all particles
	vector<ofxWWTweetParticle>::iterator it = tweets.begin();
	while(it != tweets.end()) {
		ofxWWTweetParticle& tweet = (*it);
		tweet.dot_opacity = p;
		tweet.force += tweet.static_force;
		tweet.force.y += (manager.tweetFlowSpeed + tweet.speedAdjust) * (1-tweet.clampedSelectionWeight);
		++it;
	}	
}

