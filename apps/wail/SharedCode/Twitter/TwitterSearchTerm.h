#ifndef ROXLU_TWITTER_SEARCH_TERMH
#define ROXLU_TWITTER_SEARCH_TERMH

#include "Twitter.h"
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
		printf("[search term]: %s, %s, %c\n", user.c_str(), search_term.c_str(), (is_used) ? 'y' : 'n');
	}
	
	bool operator==(const TwitterSearchTerm& o) {
		return (user == o.user && search_term == o.search_term);
	}
	
	string user;	
	string search_term;
	bool is_used;

};

#endif