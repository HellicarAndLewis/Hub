#ifndef TWEET_PROVIDER_STREAMH
#define TWEET_PROVIDER_STREAMH

#include "Twitter.h"
#include "TweetProvider.h"
#include "TwitterApp.h"
#include "IEventListener.h"

// Converting to ascii
#include "Poco/TextConverter.h"
#include "Poco/ASCIIEncoding.h"
#include "Poco/UTF8Encoding.h"
#include <iostream>

using Poco::TextConverter;
using Poco::ASCIIEncoding;
using Poco::UTF8Encoding;

class TweetProviderStream : public TweetProvider, public roxlu::twitter::IEventListener {

public:

	TweetProviderStream(TwitterApp& app);
	virtual void update();
	virtual void activate();
	virtual void deactivate();

	
	virtual void onStatusUpdate(const rtt::Tweet& tweet);
	virtual void onStatusDestroy(const rtt::StatusDestroy& destroy);
	virtual void onStreamEvent(const rtt::StreamEvent& event);

private:

	TwitterApp& app;

};
#endif