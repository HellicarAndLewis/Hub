#ifndef TWITTER_DBH
#define TWITTER_DBH

#include "Twitter.h"
#include "Database.h"
#include "ofMain.h"

namespace rtt = roxlu::twitter::type;

class TwitterDB { 
public:
	TwitterDB();
	bool open(const string& name);
	bool createTables();
	
	
	// follower
	bool insertFollower(const rtt::StreamEvent& event);
	bool removeFollower(const rtt::StreamEvent& event);
	bool getFollowers(vector<string>& result);
	
	// tweets
	bool insertTweet(const rtt::Tweet& tweet);
	bool getTagID(const string& tag, int& result);
	bool getTweetsWithTag(const string& tag, int howMany, vector<rtt::Tweet>& result);
	bool getTweetsNewerThan(int age, int howMany, vector<rtt::Tweet>& result);
	bool getTweetsWithSearchTerm(const string& q, vector<rtt::Tweet>& result);

private:
	roxlu::Database db;	
};


#endif