#pragma once

#include "ofMain.h"
#include "TwitterApp.h"
#include "FuzzySelection.h"



class ofxWWTweetParticleManager;
class ofxWWTweetParticle {
  public:
  	enum ofxWWTweetParticleState {
		 STATE_DEFAULT
		,STATE_HIGHLIGHT
		,STATE_HIDING
	};
	ofxWWTweetParticle();

	ofxWWTweetParticleManager* manager;
	rtt::Tweet tweet;
	
	void setTweet(rtt::Tweet tweet);
	void update();
	void drawDot();
	void drawText();
	bool isDrawingText();
	void drawDebug();
	void setState(int state);
	
	//controlled through update
	ofVec2f pos;
	ofVec2f lastPos;
	ofVec2f force;
	
	float selectionWeight; //influence from hand
	float clampedSelectionWeight;
	
	float scale;
	float opacity;	
	float deathAttenuation; //used to fall off wall force
	float createdTime;
	bool dead;
	
	//calculated on set
	bool isTwoLines;

	ofRectangle boundingRect; //updated every frame
	float totalWidth;
	float totalHeight;
	
	float speedAdjust;
	
	bool isSearchTweet;
	bool useBurstOne;

	string lineOne;
	string lineTwo;

	float lineOneWidth;
	float lineOneHeight;
	float lineTwoWidth;
	float lineTwoHeight;
	float userNameWidth;
	float userNameHeight;

	float atSignWidth;
	float atSignHeight;
	
	ofVec2f getBoundingCorner(int cornerIndex); //0-4 top left, top right, bot left, bottom right

	float dot_opacity; 
	ofVec2f static_force; 
	uint32_t delete_id; // used to remove a tweet from screen.
	int state;
  protected:
	
	FuzzySelection smoothedSelectionWeight;
	
	ofVec2f getUserDrawPos();
	ofVec2f getTweetLineOneDrawPos();
	ofVec2f getTweetLineTwoDrawPos();
	ofVec2f getAtDrawPos();
	
	float typePlacementTweenPos();
	void recalculateBoundingRects();

	void drawStarImage(float alpha);
	
	float whichImage;
	static const int NUM_DOT_IMAGES = 5;
	static ofImage *dotImages[NUM_DOT_IMAGES];
	float imageScale;
	

	static ofImage *highlightImage;

	
	float lifetime;
	float highlight_duration; // general timer...

};

inline void ofxWWTweetParticle::setState(int newState) {
	state = newState;
	if(state == STATE_HIGHLIGHT) {	
		lifetime = ofGetElapsedTimeMillis() + highlight_duration;
	}
}


inline bool ofxWWTweetParticle::isDrawingText() {
	return opacity > 0;
}
