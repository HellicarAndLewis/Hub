#ifndef TWITTER_EVENT_LISTENERH
#define TWITTER_EVENT_LISTENERH

#include "IEventListener.h"
class TwitterApp;

class TwitterEventListener : public roxlu::twitter::IEventListener {
public:
	TwitterEventListener(TwitterApp& app);
	~TwitterEventListener();

	void onStatusUpdate(const rtt::Tweet& tweet);
	void onStatusDestroy(const rtt::StatusDestroy& destroy);
	void onStreamEvent(const rtt::StreamEvent& event);
	
private:
	TwitterApp& twitter_app;
};	


#endif
