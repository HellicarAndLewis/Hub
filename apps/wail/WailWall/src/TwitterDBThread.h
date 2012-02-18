#ifndef TWITTER_DB_THREADH
#define TWITTER_DB_THREADH

#include "Twitter.h"
#include "TwitterDB.h"
#include "ofMain.h"

class TwitterDBThread : public ofThread {
public:
	virtual void threadedFunction();
	bool insertTweet(const rtt::Tweet& tweet);
	
	// searching
	bool getTweetsWithTag(const string& tag, int howMany, vector<rtt::Tweet>& result);
	bool getTweetsNewerThan(int age, int howMany, vector<rtt::Tweet>& result);
	bool getTweetsWithSearchTerm(const string& q, int youngerThan, int howMany, vector<rtt::Tweet>& result);
	
	// queueing send messages
	bool insertSendQueueItem(const string& username, const string& filename, int& newID);
	bool setSendQueueItemAsSend(int queueID);
	bool getNextSendItemFromSendQueue(string& username, string& filename, int& id);
	
private:
	TwitterDB db;
};

#endif