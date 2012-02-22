#include "Force.h"
#include "ofxWWTweetParticleManager.h"
#include "ofxWWTweetParticle.h"

Force::Force(ofxWWTweetParticleManager& particleManager) 
	:manager(particleManager)
	,tweets(particleManager.tweets)
	,is_hiding(false)
{

}

bool Force::isHiding() {
	return is_hiding;
}

void Force::setShouldHide(bool should) {
	is_hiding = should;
}