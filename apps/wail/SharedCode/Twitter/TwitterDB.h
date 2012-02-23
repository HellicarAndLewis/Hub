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
	
	// tweets
	bool insertTweet(const rtt::Tweet& tweet);
	bool deleteTweetByTweetID(const string& id);
	bool getTweetsNewerThan(int age, int howMany, vector<rtt::Tweet>& result);
	bool getTweetsWithSearchTerm(const string& q, time_t olderThan, int howMany,vector<rtt::Tweet>& result);
	
	// send queue
	bool insertSendQueueItem(const string& username, const string& filename, int& newID);
	bool setSendQueueItemAsSend(int queueID);
	bool getNextSendItemFromSendQueue(string& username, string& filename, int& id);

private:
	roxlu::Database db;	
};


#endif