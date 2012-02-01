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
	bool insertTweet(const rtt::Tweet& tweet);
	bool insertFollower(const rtt::StreamEvent& event);
	bool removeFollower(const rtt::StreamEvent& event);
	bool getFollowers(vector<string>& result);

private:
	roxlu::Database db;	
};


#endif