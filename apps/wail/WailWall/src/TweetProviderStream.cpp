#include "TweetProviderStream.h"
TweetProviderStream::TweetProviderStream(TwitterApp& app) 
	:app(app)
{
}

void TweetProviderStream::update() {
	//printf("stream provider update\n");
}

void TweetProviderStream::onStatusUpdate(const rtt::Tweet& tweet) {
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
	if(app.containsBadWord(tweet.text, bad_word)) {
		if(isEnabled()) { // only log when enabled
			printf("[censored][%s] %s\n",bad_word.c_str(), tweet.getText().c_str());
		}	
		
		//printf("# [ censored ] : %s\n", tweet.text.c_str());
		//printf("[censored]\n");
		return;
	}
//	printf("[ok]\n");
	//printf("[ok] %s \n", tweet.getText().c_str());

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
		app.onNewSearchTerm(tweet, search_query);
	}
	else {
		// store tweet in DB so the visual app can fetch it.
		app.insertTweet(tweet);
	}
	
	if(isEnabled()) {
		printf("--------------------------------------------------\n");
		onNewTweet(tweet);
	}
}

void TweetProviderStream::activate() {
}

void TweetProviderStream::deactivate() {
}

void TweetProviderStream::onStatusDestroy(const rtt::StatusDestroy& destroy) {

}

void TweetProviderStream::onStreamEvent(const rtt::StreamEvent& event) {

}