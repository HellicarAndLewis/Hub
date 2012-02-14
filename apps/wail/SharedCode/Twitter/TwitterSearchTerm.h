#ifndef ROXLU_TWITTER_SEARCH_TERMH
#define ROXLU_TWITTER_SEARCH_TERMH

#include <string>
using std::string;

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
		printf("----------\n");
	}
	
	bool operator==(const TwitterSearchTerm& o) {
		printf("Check same: %s = %s\n", o.search_term.c_str(), search_term.c_str());
		return (user == o.user && search_term == o.search_term);
	}
	
	string user;
	string search_term;
	bool is_used;

};

#endif