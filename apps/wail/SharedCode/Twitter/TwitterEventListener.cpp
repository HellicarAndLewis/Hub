#include "TwitterEventListener.h"
#include "TWitterApp.h"

TwitterEventListener::TwitterEventListener(TwitterApp& app)
	:twitter_app(app)
{

}

TwitterEventListener::~TwitterEventListener() {
}

void TwitterEventListener::onStatusUpdate(const rtt::Tweet& tweet) {
	printf("text: %s\n", tweet.getText().c_str());
	twitter_app.getDB().insertTweet(tweet);
}

void TwitterEventListener::onStatusDestroy(const rtt::StatusDestroy& destroy) {
}

/*
	User target;					// who is affected (i.e. removed, added) or owns the affected object
	TargetObject target_object;		// the "changed" object; i.e. a list
	User source; 					// who initiated the event
	string event;					// what kind of event.
*/

void TwitterEventListener::onStreamEvent(const rtt::StreamEvent& event) {
	printf("Stream event\n");
	if(event.event == "list_member_removed") {
		twitter_app.getDB().removeFollower(event);
		printf("List member: %s removed from list: %s\n", event.target.screen_name.c_str(), event.target_object.name.c_str());
	}
	else if(event.event == "list_member_added") {
		twitter_app.getDB().insertFollower(event);
		printf("List member: %s added to list: %s\n", event.target.screen_name.c_str(), event.target_object.name.c_str());
	}
	
	// test
	printf("\n\n\n--------- get followers ----------------\n");
	vector<string> r;
	twitter_app.getFollowers(r);
	printf("-------------------\n");

}
