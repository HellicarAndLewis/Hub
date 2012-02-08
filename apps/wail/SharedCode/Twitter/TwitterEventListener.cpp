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
	rtt::Tweet clean_tweet = tweet;
	
	twitter_app.cleanupBadWords(clean_tweet.text, " [censored] ");

	printf("> %s\n", clean_tweet.getText().c_str());
	string search_for = "dewarshub";
	size_t search_len = search_for.size();
	
	twitter_app.getDB().insertTweet(clean_tweet);
	if(clean_tweet.user_mentions.size()) {
		for(int i = 0; i < clean_tweet.user_mentions.size(); ++i) {
			string name = clean_tweet.user_mentions[i];
			if(name.length() != search_len) {
				continue;
			}
			// @todo pass to the visualisation app.
	
			std::transform(name.begin(), name.end(), name.begin(), ::tolower);
			if(name == search_for) {
				// tokenize
				std::istringstream ss(clean_tweet.getText());
				string token;
				ss >> token;
				while(ss.good()) {
					ss >> token; 
					twitter_app.onNewSearchTerm(clean_tweet, token);
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
