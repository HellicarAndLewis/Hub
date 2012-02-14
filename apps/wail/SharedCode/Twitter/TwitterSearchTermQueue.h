#ifndef ROXLU_TWITTER_SEARCH_TERM_QUEUEH
#define ROXLU_TWITTER_SEARCH_TERM_QUEUEH

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