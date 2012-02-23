#include "TweetProvider.h"

TweetProvider::TweetProvider(int providerKind) 
	:kind_of_provider(providerKind)
{
}

void TweetProvider::addListener(TweetProviderListener* l) {
	listeners.push_back(l);
}

void TweetProvider::onNewTweet(const rtt::Tweet& tweet) {
	for(int i = 0; i < listeners.size(); ++i) {
		listeners[i]->onNewTweet(tweet);
	}
}
