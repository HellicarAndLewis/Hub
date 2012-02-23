#include "TweetProviderDB.h"
TweetProviderDB::TweetProviderDB(TwitterApp& app)
	:app(app)
	,should_create_new_tweet_on(0)
	,spawn_delay(50)
	,TweetProvider(TweetProvider::PROVIDER_DB)
{
}

void TweetProviderDB::update() {
	if(!is_enabled) {
		return;
	}	
	
	if(app.retrieveSearchResultsFromThread(found_tweets)) {
	}

	int now = ofGetElapsedTimeMillis();
	if(now > should_create_new_tweet_on) {
		should_create_new_tweet_on = now + spawn_delay;
		if(found_tweets.size() > 0) {
			tweet_index = ++tweet_index % found_tweets.size();
			//printf("> PASSING: %s\n", found_tweets[tweet_index].getText().c_str());
			onNewTweet(found_tweets[tweet_index]);
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


// TODO add number of new spawned particles/tweets to setting
void TweetProviderDB::fillWithTweetsWhichContainTerm(const string& term) {
	found_tweets.clear();
	tweet_index = 0;
	app.getTweetsWithSearchTerm(term, 200);
}

void TweetProviderDB::activate() {
	should_create_new_tweet_on = ofGetElapsedTimeMillis() + spawn_delay;
}

void TweetProviderDB::deactivate() {
}