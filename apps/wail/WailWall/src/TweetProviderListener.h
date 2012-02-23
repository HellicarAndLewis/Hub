#ifndef TWEET_PROVIDER_LISTENERH
#define TWEET_PROVIDER_LISTENERH
#include "Twitter.h"

namespace rt = roxlu::twitter;
namespace rtt = roxlu::twitter::type;

class TweetProviderListener {
public:
	virtual void onNewTweet(const rtt::Tweet& tweet) = 0;
};

#endif