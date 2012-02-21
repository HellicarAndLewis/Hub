#ifndef FORCEH
#define FORCEH

class ofxWWTweetParticleManager;

class Force {
public:

	Force(ofxWWTweetParticleManager& manager); 
	
	virtual void update() = 0;
	
protected:
	
	ofxWWTweetParticleManager& manager;
};
#endif