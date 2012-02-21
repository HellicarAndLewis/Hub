#include "TweetProviderDB.h"
TweetProviderDB::TweetProviderDB(TwitterApp& app)
	:app(app)
	,should_create_new_tweet_on(0)
{
}

void TweetProviderDB::update() {
	if(!is_enabled) {
		return;
	}	
	int now = ofGetElapsedTimeMillis();
	if(now > should_create_new_tweet_on) {
		should_create_new_tweet_on = now + 10;
		if(found_tweets.size() > 0) {
			tweet_index = ++tweet_index % found_tweets.size();
			onNewTweet(found_tweets[tweet_index]);
		}
		else {
			printf("+++++++++++++++++++++++++++++++++++++++++++++\n");
			printf("          we did not found                   \n");
			printf("+++++++++++++++++++++++++++++++++++++++++++++\n");
		}
	}
	
}


void TweetProviderDB::setSearchInfoForNewParticles(
							 const string& username
							,const string& term
)
{
	current_search_term = term;
	current_username = username;
}

void TweetProviderDB::fillWithTweetsWhichContainTerm(const string& term) {
	printf("============================ search terms ============================\n");
	found_tweets.clear();
	app.getTweetsWithSearchTerm(term, 100000, 20, found_tweets);
	for(int i = 0; i < found_tweets.size(); ++i) {
		printf("[found] (%s) %s\n"
				,found_tweets[i].getScreenName().c_str()
				,found_tweets[i].getText().c_str()
		);
	}
	printf("+++++++++++++++++++++++++++++ %zu ++++++++++++++++++++++++++++\n", found_tweets.size());
	
}

void TweetProviderDB::activate() {
	should_create_new_tweet_on = ofGetElapsedTimeMillis() + 10;
}

void TweetProviderDB::deactivate() {
}