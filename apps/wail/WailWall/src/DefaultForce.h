#ifndef DEFAULTFORCEH
#define DEFAULTFORCEH

#include "Force.h"
#include "ofMain.h"

class DefaultForce : public Force {

public:

	DefaultForce(ofxWWTweetParticleManager& manager);
	virtual void hide();
	virtual bool isReadyWithHiding();

	virtual void activateParticle(ofxWWTweetParticle& p);
	virtual void activate();
	virtual void deactivateParticle(ofxWWTweetParticle& p);
	virtual void deactivate();
	
	virtual void show();
	
	//float deactivated_on;
	float hide_force;
	float should_hide_on;
	float hide_duration_millis;
	ofVec2f center;
};

#endif