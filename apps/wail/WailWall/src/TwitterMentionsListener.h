#ifndef TWITTER_MENTIONS_LISTENERH
#define TWITTER_MENTIONS_LISTENERH

#include "Twitter.h"

class TwitterMentionsListener {
public:
	virtual void onNewSearchTermFromPollingAPI(const rtt::Tweet& tweet, const string& term) = 0;
};

#endif