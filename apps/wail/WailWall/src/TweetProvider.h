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
	void enable();
	void disable();
	bool isEnabled();	
	
	bool is_enabled;
	vector<TweetProviderListener*> listeners;
};

inline void TweetProvider::enable() {
	is_enabled = true;
	activate();
}

inline void TweetProvider::disable() {
	is_enabled = false;
	deactivate();
}

inline bool TweetProvider::isEnabled() {
	return is_enabled;
}

#endif