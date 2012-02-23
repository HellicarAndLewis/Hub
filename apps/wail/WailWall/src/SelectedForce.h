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
};

#endif