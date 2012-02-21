#ifndef TWEET_PROVIDER_DBH
#define TWEET_PROVIDER_DBH

#include "TweetProvider.h"
#include "TwitterApp.h"
#include "ofMain.h"


class TweetProviderDB : public TweetProvider {
public:
	TweetProviderDB(TwitterApp& app);
	virtual void update();
	virtual void activate();
	virtual void deactivate();
	void setSearchInfoForNewParticles(const string& username, const string& term);
	void fillWithTweetsWhichContainTerm(const string& term);
	
private:
	string current_search_term;
	string current_username;
	TwitterApp& app;
	vector<rtt::Tweet> found_tweets;
	int should_create_new_tweet_on;
	int tweet_index;
	float spawn_delay; // ms
};


#endif