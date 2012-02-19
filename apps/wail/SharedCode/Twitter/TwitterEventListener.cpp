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
/**
 * 
 * When a new tweet arrives at the stream, this function is called. Here we
 * store the tweet into the database so the visual part of the app can fetch
 * this when it needs to. 
 * 
 * We also check if the tweet contains a search term. Search terms must follow
 * this format:
 * 
 *
 * 					@dewarshub [the search terms]
 *
 *
 * Some examples:
 * -------------- 
 * "@dewarshub search me!";  --> "search me!"
 * "wow did you people see @dewarshub last night!"; --> No search term!
 * "@dewars wow did you people see last night!"; --> No search term
 " "@dewarshub economy" --> economy
 *
 */
void TwitterEventListener::onStatusUpdate(const rtt::Tweet& tweet) {
	string bad_word;
	bool to_dewarshub = false;
	for(int i = 0; i < tweet.user_mentions.size(); ++i) {
		if(tweet.user_mentions[i] == "dewarshub") {
			to_dewarshub = true;
			break;
		}
		//printf("> %s\n", tweet.user_mentions[i].c_str());
	}
	if(to_dewarshub) {
		printf("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
		printf("%s\n", tweet.text.c_str());
		printf("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	}
	if(twitter_app.containsBadWord(tweet.text, bad_word)) {
		//printf("[censored][%s] %s\n",bad_word.c_str(), tweet.getText().c_str());
		//printf("# [ censored ] : %s\n", tweet.text.c_str());
		printf("[censored]\n");
		return;
	}
//	printf("[ok]\n");
	printf("[ok] %s \n", tweet.getText().c_str());

	// Check for search term; we first lowercase the tweet.
	string search_query;
	string tweet_text_lower = tweet.getText();
	std::transform(
		 tweet_text_lower.begin()
		,tweet_text_lower.end()
		,tweet_text_lower.begin()
		,::tolower
	);
	
	// Check if it's a correct search term:
	pcrecpp::RE re("^@dewarshub (.*)$");
	re.FullMatch(tweet_text_lower, &search_query);
	if(search_query.length()) {
		printf("[search]: '%s'\n", search_query.c_str());
		twitter_app.onNewSearchTerm(tweet, search_query);
	}
	else {
		// store tweet in DB so the visual app can fetch it.
		twitter_app.insertTweet(tweet);
	}
}

void TwitterEventListener::onStatusDestroy(const rtt::StatusDestroy& destroy) { }
void TwitterEventListener::onStreamEvent(const rtt::StreamEvent& event) { }
