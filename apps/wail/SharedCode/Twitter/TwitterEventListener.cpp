#include <algorithm>
#include <sstream>
#include "TwitterEventListener.h"
#include "TWitterApp.h"

TwitterEventListener::TwitterEventListener(TwitterApp& app)
	:twitter_app(app)
{

}

TwitterEventListener::~TwitterEventListener() {
}

void TwitterEventListener::onStatusUpdate(const rtt::Tweet& tweet) {
	//printf("text: %s\n", tweet.getText().c_str());
	
	string search_for = "dewarshub";
	size_t search_len = search_for.size();
	
	twitter_app.getDB().insertTweet(tweet);
	if(tweet.user_mentions.size()) {
		for(int i = 0; i < tweet.user_mentions.size(); ++i) {
			string name = tweet.user_mentions[i];
			if(name.length() != search_len) {
				continue;
			}
			// @todo pass to the visualisation app.
	
			std::transform(name.begin(), name.end(), name.begin(), ::tolower);
			if(name == search_for) {
				// tokenize
				std::istringstream ss(tweet.getText());
				string token;
				while(ss.good()) {
					ss >> token;
					printf(":: token: %s\n", token.c_str());
				}
			}
		}
	}
}

void TwitterEventListener::onStatusDestroy(const rtt::StatusDestroy& destroy) {
}


void TwitterEventListener::onStreamEvent(const rtt::StreamEvent& event) {
	/*
	if(event.event == "list_member_removed") {
		twitter_app.getDB().removeFollower(event);
	}
	else if(event.event == "list_member_added") {
		twitter_app.getDB().insertFollower(event);
	}
	*/
}
