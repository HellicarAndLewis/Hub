#ifndef SelectedForceH
#define SelectedForceH

#include "ofMain.h"
#include "Force.h"

class ofxWWTweetParticleManager;
class ofxWWTweetParticle;

class SelectedForce : public Force {
public:

	SelectedForce(ofxWWTweetParticleManager& manager);
	
	virtual void hide();
	virtual bool isReadyWithHiding();
	
	virtual void activateParticle(ofxWWTweetParticle& p);
	virtual void activate();
	virtual void deactivateParticle(ofxWWTweetParticle& p) ;
	virtual void deactivate();
	
	virtual void show();
	
	/*
	float force;
	float hide_duration_millis;
	float should_hide_on;
	ofVec2f center;
	*/
};

#endif