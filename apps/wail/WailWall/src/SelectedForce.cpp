#include "SelectedForce.h"
#include "ofxWWTweetParticleManager.h"
#include "ofxWWTweetParticle.h"

/*
,should_hide_on(0)
	,hide_duration_millis(800)
	,force(58.5)
*/

SelectedForce::SelectedForce(ofxWWTweetParticleManager& manager) 
	:Force(manager)
{
	//center.set(manager.simulationWidth * 0.5,manager.simulationHeight * 0.5);
}

void SelectedForce::show() {
	/*
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
	*/
}

// is called when new particles are added to the simulation after switching
void SelectedForce::activateParticle(ofxWWTweetParticle& p) {
	p.pos.x = ofRandom(0,manager.simulationWidth);
	p.pos.y = ofRandom(0, manager.simulationHeight);
}

void SelectedForce::deactivateParticle(ofxWWTweetParticle& p) {
}

void SelectedForce::activate() {
	// get the search terms....
	

	/*
	should_hide_on = ofGetElapsedTimeMillis() + hide_duration_millis; // TODO make setting
	vector<ofxWWTweetParticle>::iterator it = tweets.begin();
	while(it != tweets.end()) {
		ofxWWTweetParticle& p = (*it);
		p.static_force = (p.pos - center).normalize() * force;
		++it;
	}
	*/
	//static_force
}

void SelectedForce::hide() {
}

bool SelectedForce::isReadyWithHiding() {
	return true;
}

void SelectedForce::deactivate() {
}