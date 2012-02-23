#ifndef FORCEH
#define FORCEH

#include <vector>
using std::vector;

class ofxWWTweetParticleManager;
class ofxWWTweetParticle;

class Force {
public:

	Force(ofxWWTweetParticleManager& manager); 
	

	bool isHiding();
	void setShouldHide(bool should);
	virtual bool isReadyWithHiding() = 0;
	
	virtual void show() = 0;	
	virtual void hide() = 0;
	
	virtual void activateParticle(ofxWWTweetParticle& p) = 0;
	virtual void activate() = 0;
	virtual void deactivate() = 0;
	virtual void deactivateParticle(ofxWWTweetParticle& p) = 0;

	
protected:
	bool is_hiding;
	ofxWWTweetParticleManager& manager;
	vector<ofxWWTweetParticle>& tweets;
};
#endif