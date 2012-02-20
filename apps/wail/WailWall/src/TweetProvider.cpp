#include "TweetProvider.h"

void TweetProvider::addListener(TweetProviderListener* l) {
	listeners.push_back(l);
}

void TweetProvider::onNewTweet(const rtt::Tweet& tweet) {
	for(int i = 0; i < listeners.size(); ++i) {
		listeners[i]->onNewTweet(tweet);
	}
}
