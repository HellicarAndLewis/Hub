#ifndef TWITTER_DB_THREADH
#define TWITTER_DB_THREADH

#include "Twitter.h"
#include "TwitterDB.h"
#include "ofMain.h"
#include <deque>

struct TwitterDBThreadTask {
	TwitterDBThreadTask(int task)
		:kind_of_task(task)
	{
	}
	
	enum {
		 TASK_NONE
		,TASK_SEARCH
	};
	int kind_of_task;
	
	// values for search task
	int search_younger_than;
	int search_howmany;
	string search_term;
	
	void setSearchYoungerThan(int d) { search_younger_than = d; }
	void setSearchHowMany(int d) { search_howmany = d;}
	void setSearchTerm(const string& s) { search_term = s;}

};

struct TweetTimestampSorter {
	bool operator()(const rtt::Tweet& a, const rtt::Tweet& b) {
		return a.created_at_timestamp > b.created_at_timestamp;
	}
};

class TwitterDBThread : public ofThread {
public:
	TwitterDBThread();
	virtual void threadedFunction();
	bool insertTweet(const rtt::Tweet& tweet);
	void addTask(TwitterDBThreadTask task);
	
	// searching
	bool getTweetsWithTag(const string& tag, int howMany, vector<rtt::Tweet>& result);
	bool getTweetsNewerThan(int age, int howMany, vector<rtt::Tweet>& result);
	bool getTweetsWithSearchTerm(const string& q, int youngerThan, int howMany, vector<rtt::Tweet>& result);
	bool retrieveSearchResultsFromThread(vector<rtt::Tweet>& result);

	// queueing send messages
	bool insertSendQueueItem(const string& username, const string& filename, int& newID);
	bool setSendQueueItemAsSend(int queueID);
	bool getNextSendItemFromSendQueue(string& username, string& filename, int& id);
	
private:
	vector<rtt::Tweet> search_results;
	time_t search_for_older_then;
	std::deque<TwitterDBThreadTask> tasks;
	TwitterDB db;
};

#endif