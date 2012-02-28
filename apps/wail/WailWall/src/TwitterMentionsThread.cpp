#include "TwitterMentionsThread.h"
#include "pcrecpp.h"
#include <sstream>

TwitterMentionsThread::TwitterMentionsThread() {
}

void TwitterMentionsThread::setup(const string& key, const string& secret, const string& tokensFile) {
	twitter.setConsumerKey(key);
	twitter.setConsumerSecret(secret);
	if(!twitter.loadTokens(tokensFile)) {
		printf("Warning: cannot load tokens file for twitter mentions thread.\n");
	}
}

void TwitterMentionsThread::threadedFunction() {

	rcp::Collection params;
	int rate_remaining = 0;
	int rate_limit = 0;
	int rate_reset = 0;
	uint64_t last_timestamp = 0;
	params["count"] = 10;
	vector<rtt::Tweet> queried_mentions;
	vector<rtt::Tweet> newer_mentions;
	bool is_first_request = true;
	pcrecpp::RE hash_regex("#+");
	pcrecpp::RE search_regex("^@dewarshub (.*)$");
	while(true) {
		
		
		// GET MENTIONS
		// ------------------------------
		twitter.statusesMentions(&params);
		long resp_code = twitter.getHTTPResponseCode();
		
		// TODO handle out of rate states.
		if(resp_code != 200) {
			printf("mentions: wrong response code: %lu\n", resp_code);		
			sleep(15);
			continue;
		}
		
		// parse the new mentions, sort them on timestamp.
		queried_mentions.clear();
		newer_mentions.clear();
		twitter.getJSON().parseStatusArray(twitter.getResponse(), queried_mentions);
		std::sort(queried_mentions.begin(), queried_mentions.end(), TwitterMentionSorter());
		
		// filter out mentions which are newer then previous		
		for(int i = 0; i < queried_mentions.size(); ++i) {	
			rtt::Tweet& tw = queried_mentions[i];
			printf("[raw mention]  %s\n", tw.getText().c_str());
			if(tw.created_at_timestamp > last_timestamp) {
				newer_mentions.push_back(tw);
			}
		}
		
		// PARSE RATE LIMIT HEADRES
		// -------------------------------
		string str_remaining;
		std::stringstream ss_remaining;
		if(twitter.getResponseHeader("x-ratelimit-remaining", str_remaining)) {
			ss_remaining << str_remaining;
			ss_remaining >> rate_remaining;
		}

		string str_limit;
		std::stringstream ss_limit;
		if(twitter.getResponseHeader("x-ratelimit-limit", str_limit)) {
			ss_limit << str_limit;
			ss_limit >> rate_limit;
		}
		
		string str_reset;
		std::stringstream ss_reset;
		if(twitter.getResponseHeader("x-ratelimit-reset", str_reset)) {
			ss_reset << str_reset;
			ss_reset >> rate_reset;
		}
		
		
		// TODO handle rate limits correctly!
		// TODO add bad word checks!! 
		// TODO check for very long search terms.		
		// TODO test... test... test..
		// TODO check sender name for bad words.
		// TODO make sure new search terms are added to the search_queue in TwitterApp .... or we use twitter as a store... 
		
		// ADD MENTIONS AS SEARCH TERMS
		// ----------------------------
		if(newer_mentions.size() > 0) {			
	
			last_mention = newer_mentions.at(0);			
			params["since_id"] = last_mention.getTweetID();
			
			// notice the listeners about this new search term
			lock();
				std::vector<rtt::Tweet>::reverse_iterator it = newer_mentions.rbegin();
				while(it != newer_mentions.rend()) {	
										
					// parse bad word out of it.
					rtt::Tweet& tweet = *it;
					string search_query;
					string tweet_text_lower = tweet.getText();
					std::transform(
						 tweet_text_lower.begin()
						,tweet_text_lower.end()
						,tweet_text_lower.begin()
						,::tolower
					);
					
					tweet_text_lower = trim(tweet_text_lower);
					
					// Check if it's a correct search term:
					search_regex.FullMatch(tweet_text_lower, &search_query);
					if(search_query.length()) {
						printf("[filtered mention]: %s\n", search_query.c_str());
						StringTokenizer tokens(search_query, " ",Poco::StringTokenizer::TOK_IGNORE_EMPTY);
						if(tokens.count() > 0 && tokens[0].length() <= 20) {
							string s = "@dewarshub search";
							string token = tokens[0];
							hash_regex.GlobalReplace("", &token);
						
							search_query = "#" +token;
							TwitterMentionSearchTerm twit_search_term = {tweet, search_query, is_first_request};
                            search_terms.push_back(twit_search_term);
						}
						else {
							printf("[search term invalid] %s\n", search_query.c_str());
						}
					}
					++it;
				}
			unlock();
			is_first_request = false;
		}
		// TODO put the mentions wait in a thread (between 10-20 seconds)
		sleep(15);
	}
}

void TwitterMentionsThread::simulateSearch(const string& term) {
	lock();
		rtt::Tweet tweet;
		string t = "@dewarshub " +term;
		tweet.setText(t);
		tweet.setScreenName("roxlutest");
		TwitterMentionSearchTerm twit_search_term = {tweet, "#"+term, false};
		search_terms.push_back(twit_search_term);
	unlock();
}

bool TwitterMentionsThread::getSearchTerms(vector<TwitterMentionSearchTerm>& result) {
	lock();
	bool has = search_terms.size() > 0;
	if(!has) {
		unlock();
		return false;
	}
	else {
		result = search_terms;
		search_terms.clear();
		unlock();
	}
	return true;
}
