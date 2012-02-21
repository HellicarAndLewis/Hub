#ifndef ROXLU_TWITTER_SEARCH_TERM_QUEUEH
#define ROXLU_TWITTER_SEARCH_TERM_QUEUEH

// TODO after some long testing we realized the twitter stream wasn't 
// fast enough with providing new search terms so we switched over to a 
// system where we're polling the REST API of twitter so we don't really need
// to store the previous searches anymore because we're using the mentions
// to @dewarshub...  So we basically need to remove this from all of the code.

#include "TwitterSearchTerm.h"
#include <fstream>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace roxlu {

struct TwitterSearchTermCompare {
	TwitterSearchTermCompare(const string& u, const string& q):u(u),q(q){}
	bool operator()(const TwitterSearchTerm* a) const {
		return a->user == u && a->search_term == q;
	}
	const string& q;
	const string& u;
};


// Queue for added search terms
// ----------------------------
class TwitterSearchTermQueue {
public:
	TwitterSearchTermQueue();
	~TwitterSearchTermQueue();
	void setup(const string& filePath);
	bool addSearchTerm(const string& user, const string& searchTerm);
	bool getUnusedSearchTerms(vector<TwitterSearchTerm*>& result);
	bool setSearchTermAsUsed(const string& user, const string& searchTerm);
	bool save();
	bool load();
private:
	string filepath;	
	vector<TwitterSearchTerm*> terms;
};

}

#endif