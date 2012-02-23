#include "SelectedForce.h"
#include "ofxWWTweetParticleManager.h"
#include "ofxWWTweetParticle.h"

SelectedForce::SelectedForce(ofxWWTweetParticleManager& manager) 
	:Force(manager)
{
}

void SelectedForce::show() {
}

// is called when new particles are added to the simulation after switching
void SelectedForce::activateParticle(ofxWWTweetParticle& p) {
	p.pos.x = ofRandom(0,manager.simulationWidth);
	p.pos.y = ofRandom(0, manager.simulationHeight);
	p.setState(ofxWWTweetParticle::STATE_HIGHLIGHT);
}

void SelectedForce::deactivateParticle(ofxWWTweetParticle& p) {
}

void SelectedForce::activate() {
	printf("ACTIVATE SELECTED FORCE\n");
	vector<ofxWWTweetParticle>::iterator it= manager.tweets.begin();
	while(it!=manager.tweets.end()) {
		if((*it).state!=ofxWWTweetParticle::STATE_HIGHLIGHT) {
			printf("..\n");
			it = manager.tweets.erase(it);
		} else {
			it++;
		}
	}
}

void SelectedForce::hide() {
}

bool SelectedForce::isReadyWithHiding() {
	return true;
}

void SelectedForce::deactivate() {
}