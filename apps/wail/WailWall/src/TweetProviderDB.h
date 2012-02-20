#ifndef TWEET_PROVIDER_DBH
#define TWEET_PROVIDER_DBH

#include "TweetProvider.h"
#include "TwitterApp.h"

class TweetProviderDB : public TweetProvider {
public:
	TweetProviderDB(TwitterApp& app);
	virtual void update();
	void setSearchTermToUseForQueries(const string& term);
	
private:
	string current_search_term;
	TwitterApp& app;
};


inline void TweetProviderDB::setSearchTermToUseForQueries(const string& term) {
	current_search_term;
}
#endif