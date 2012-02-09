#ifndef ROXLU_TWITTER_SEARCH_TERM_QUEUEH
#define ROXLU_TWITTER_SEARCH_TERM_QUEUEH

#include <fstream>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace roxlu {

// Search term
// -------------------------
struct TwitterSearchTerm {
	TwitterSearchTerm(const string& u, const string& q, bool used)
		:user(u)
		,search_term(q)
		,is_used(used)
	{
	}
	
	void print() {
		printf("user: %s\n", user.c_str());
		printf("search term: %s\n", search_term.c_str());
		printf("is used: %c\n", (is_used) ? 'y' : 'n');
	}
	
	string user;
	string search_term;
	bool is_used;

};


// Queue for added search terms
// ----------------------------
class TwitterSearchTermQueue {
public:
	TwitterSearchTermQueue();
	~TwitterSearchTermQueue();
	void setup(const string& filePath);
	void addSearchTerm(const string& user, const string& searchTerm);
	bool getUnusedSearchTerms(vector<TwitterSearchTerm*>& result);
	
	bool save();
	bool load();
private:
	string filepath;	
	vector<TwitterSearchTerm*> terms;
};

}

#endif