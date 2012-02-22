#include <algorithm>
#include <sstream>
#include "TwitterEventListener.h"
#include "TWitterApp.h"
#include "pcrecpp.h"
		
TwitterEventListener::TwitterEventListener(TwitterApp& app)
	:twitter_app(app)
{
}

TwitterEventListener::~TwitterEventListener() {
}

void TwitterEventListener::onStatusUpdate(const rtt::Tweet& tweet) {
}

void TwitterEventListener::onStatusDestroy(const rtt::StatusDestroy& destroy) { }
void TwitterEventListener::onStreamEvent(const rtt::StreamEvent& event) { }
