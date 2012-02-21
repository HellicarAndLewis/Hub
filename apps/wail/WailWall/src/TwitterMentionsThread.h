#ifndef TWITTER_MENTIONS_THREADH
#define TWITTER_MENTIONS_THREADH

//#include "TwitterMentionsListener.h"
//#include "TwitterSearchTerm.h"
#include "Twitter.h"
#include "ofMain.h"
#include <string>
#include <Poco/String.h>
#include <Poco/StringTokenizer.h>

using Poco::trim;
using Poco::StringTokenizer;
using std::string;
namespace rt = roxlu::twitter;

// when sorting with this, the first entry is newest.
struct TwitterMentionSorter {
	TwitterMentionSorter() {}
	bool operator()(const rtt::Tweet& a, const rtt::Tweet& b) {
		return a.created_at_timestamp > b.created_at_timestamp;
	}
};

struct TwitterMentionSearchTerm {
	rtt::Tweet tweet;
	string search_term;
	bool is_old;
};

class TwitterMentionsThread : public ofThread {
public:
	
	TwitterMentionsThread();
	
	void setup(const string& key, const string& secret, const string& tokensFile);
	
	virtual void threadedFunction();
	
	bool getSearchTerms(vector<TwitterMentionSearchTerm>& result);
	
//	void addListener(TwitterMentionsListener* l) {
//		lock();
//			mention_listeners.push_back(l);
//		unlock();
//	}
	
private:

	rt::Twitter twitter;
	rtt::Tweet last_mention;
	//vector<TwitterMentionsListener*> mention_listeners;
	vector<TwitterMentionSearchTerm> search_terms;
};

#endif