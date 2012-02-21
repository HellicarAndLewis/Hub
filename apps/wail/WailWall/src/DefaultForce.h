#ifndef DEFAULTFORCEH
#define DEFAULTFORCEH

#include "Force.h"

class DefaultForce : public Force {

public:
	DefaultForce(ofxWWTweetParticleManager& manager);
	virtual void update();
};

#endif