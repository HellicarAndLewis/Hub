/*
 * @author James George
 * @author Diederick Huijbers // roxlu <diederick@apollomedia.nl>
 * @author Marek 
 */
 
#pragma once
#define USE_FTGL

#include "ofMain.h"
#include "ofxWWTweetParticle.h"
#include "ofxWWSearchTerm.h"
#include "TwitterApp.h"
#include "TwitterSearchTerm.h"
#include "ofxFTGLFont.h"
#include "ofxMPMFluid.h"
#include "KinectTouchListener.h"

#include "TweetProvider.h"
#include "TweetProviderStream.h"
#include "TweetProviderDB.h"
#include "TweetProviderListener.h"
#include "ofxWWSearchTermManager.h"
#include "SearchLayerListener.h"
#include "Forces.h"


class ofxWWRenderer;

class ofxWWTweetParticleManager : public TweetProviderListener, public SearchLayerListener {

  public:

	ofxWWTweetParticleManager();
	
	void setup(ofxWWRenderer* ren);
	
	void setupGui();
	
	void update();

	void renderTweets();
	
	void renderSearchTerms();

	void renderConnections();
	
	void onNewTweet(const rtt::Tweet& tweet);

	void onNewSearchTerm(TwitterAppEvent& event);
	
	// implementing: SearchLayerListener
	virtual void onSearchTermSelected(const SearchTermSelectionInfo& term);
	virtual void onAllSearchTermsDeselected();
	
	TwitterApp& getTwitterApp();
	

	ofxMPMFluid* fluidRef;
	map<int,KinectTouch>* blobsRef;
	

	ofxWWSearchTermManager searchTermManager;
	ofxWWSearchTermManager& getSearchTermManager();
	bool getTweetWithDeleteID(uint32_t id, ofxWWTweetParticle& result);
	void removeTweetWithDeleteID(uint32_t id);
	
	
	#ifdef USE_FTGL
		ofxFTGLFont sharedTweetFont;
		ofxFTGLFont sharedUserFont;
		ofxFTGLFont sharedSearchFont;
	#else	
		ofTrueTypeFont sharedTweetFont;
		ofTrueTypeFont sharedUserFont;
		ofTrueTypeFont sharedSearchFont;
	#endif
	
	
	
	// this is how much inactive time triggers a call to action.
	float 	callToActionTime;
	
	bool 	enableCaustics;
	float 	causticFadeSpeed;
	
	bool 	canSelectSearchTerms;
	float 	tweetLayerOpacity;
	float 	touchSizeScale;
	float 	touchInfluenceFalloff;
	
	float 	simulationWidth;
	float 	simulationHeight;
	
	
	bool drawTweetDebug;
	int maxTweets;
	float startFadeTime;
	float fadeDuration;
	
	float wallRepulsionDistance;
	float wallRepulsionAtten;
	float tweetRepulsionDistance;
	float tweetRepulsionAtten;
	float fluidForceScale;
		
	
	
	float tweetFlowSpeed;
	float tweetFlowVariance;
	float tweetRotateAmp;
	float tweetRotateDamp;
	float tweetChaosSpeed;
	float tweetFlowDamp;
	float tweetFlowAmp;

	
	
	
	
	
	//tweet rendering

	int userFontSize;
	int tweetFontSize;
	
	float dotSize;
	float dotShift;
	float wordWrapLength;
	float userNameYOffset;
	float tweetYOffset;
	float tweetLineSpace;
	
	bool clearTweets;
	
	
	
	

	//bool shouldTriggerScreenshot;

	
	
	vector<ofxWWTweetParticle> tweets;
	vector<ofColor> causticColors;

	//particle images
	ofImage burstOne;
	ofImage burstTwo;

	
	// search state vars



	
	void keyPressed(ofKeyEventArgs& args); //JG just used for simulating searches

	// Screenshot
	//void addCurrentRenderToScreenshotQueue();
	
	void touchUp();
	void touchDown();
	void doSearchTermSelectionTest();
	
	
	// providers for tweets
	void setCurrentProvider(TweetProvider* prov);
	TweetProvider* current_provider;
	TweetProviderStream* stream_provider;
	TweetProviderDB* db_provider;
	
	// forces 
	void setCurrentForce(Force* force);
	Force* current_force;
	Force* new_force;
	DefaultForce* default_force;
	SelectedForce* selected_force;

	
	// force GUI vars.
	
  
protected:

	TwitterApp twitter;
	

	

	
	
	void checkFonts();
	
	
	float weightBetweenPoints(ofVec2f touch, float normalizedSize, ofVec2f tweet);
	void updateTweets();
	
	
	void attemptConnection(ofVec2f pos1, float weight1, ofVec2f pos2, float weight2, float layerOpacity);
	void setRandomCausticColor(float layerOpacity);
	
	int numSearchTermTweets; //for debugging
	
	
	
	
	ofxWWTweetParticle createParticleForTweet(const rtt::Tweet& tweet);
	
	ofxWWRenderer* renderer;

	

	float lastSearchTermTime;
	
	
	// debug // test search term
	//string previous_selected_search_term;

	
};

inline ofxWWSearchTermManager& ofxWWTweetParticleManager::getSearchTermManager() {
	return searchTermManager;
}
