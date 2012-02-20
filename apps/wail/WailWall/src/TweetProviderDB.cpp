#include "TweetProviderDB.h"
TweetProviderDB::TweetProviderDB(TwitterApp& app)
	:app(app)
{
}

void TweetProviderDB::update() {
	if(!is_enabled) {
		return;
	}	
	printf("DB provider \n");
}

