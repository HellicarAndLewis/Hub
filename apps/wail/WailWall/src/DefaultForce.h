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
	
	float 	should_hide_on;
	float 	hide_duration_millis;
	ofVec2f center;
	float 	last_time_activated;
	float 	apply_startup_force_duration_seconds;
	
	float force_repulsion_rest_distance;
	float force_repulsion_spring_force;
	float force_repulsion_from_eachother;
};


#endif