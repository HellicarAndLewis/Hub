#ifndef TWEET_PROVIDERH
#define TWEET_PROVIDERH

#include <vector>
#include "TweetProviderListener.h"
#include "Twitter.h"

namespace rtt = roxlu::twitter::type;

using std::vector;

class TweetProvider {
public:

	virtual void update() = 0;
	virtual void activate() = 0;
	virtual void deactivate() = 0;
	
	void addListener(TweetProviderListener* l);
	
	void onNewTweet(const rtt::Tweet& tweet);
	
	void enable() {
		is_enabled = true;
		activate();
	}
	
	void disable() {
		is_enabled = false;
		deactivate();
	}
	
	bool isEnabled() {
		return is_enabled;
	}
	
	
	bool is_enabled;
	vector<TweetProviderListener*> listeners;
};

#endif