#include "TweetProviderStream.h"
TweetProviderStream::TweetProviderStream(TwitterApp& app) 
	:app(app)
	,TweetProvider(TweetProvider::PROVIDER_STREAM)
{
}

void TweetProviderStream::update() {
	//printf("stream provider update\n");
}

void TweetProviderStream::onStatusUpdate(const rtt::Tweet& tweet) {
	// Check for back words
	string bad_word;
	if(app.containsBadWord(tweet.text, bad_word)) {
		//printf("[censored][%s] %s\n",bad_word.c_str(), tweet.getText().c_str());
		return;
	}
	if(app.containsBadWord(tweet.getScreenName(), bad_word)) {
		//printf("[censored name][%s] %s\n", bad_word.c_str(), tweet.getScreenName().c_str());
		return;
	}
	
	//printf("> %s\n", tweet.getText().c_str());
	// Check if we've got a search term; if so we will not pass it through;
	// search terms are handled in the TwitterMentionsListener thread.
	// (we changed this search tweets werent coming into the system when it
	// was busy with the user stream.)
	string search_query;
	string tweet_text_lower = tweet.getText();
	std::transform(
		 tweet_text_lower.begin()
		,tweet_text_lower.end()
		,tweet_text_lower.begin()
		,::tolower
	);
	

	// Remove all non-ascii/latin characters
	std::string ascii_string_clean;
	UTF8Encoding utf8;
	ASCIIEncoding ascii;
	TextConverter converter(utf8, ascii,' ');
	int i = converter.convert(tweet.getText(), ascii_string_clean);
	if(ascii_string_clean != tweet.getText()) {	
		pcrecpp::RE(" +").GlobalReplace(" ", &ascii_string_clean);
		tweet_text_lower = ascii_string_clean;
	}

	// Check of search term.
	rtt::Tweet tweet_copy = tweet;
	tweet_copy.setText(ascii_string_clean);

	pcrecpp::RE re("^@dewarshub (.*)$");
	re.FullMatch(tweet_text_lower, &search_query);
	if(search_query.length()) {
		printf("[search_term_check]: '%s' (not handled here)\n", search_query.c_str());
	}
	else {
		// store tweet in DB so the visual app can fetch it.
		app.insertTweet(tweet_copy);
	}
	
	// Only when this data provider is enabled we make sure the event listeners get notified by this new tweet.
	if(isEnabled()) {
		onNewTweet(tweet_copy);
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