#include "TwitterSearchTermQueue.h"

namespace roxlu {

TwitterSearchTermQueue::TwitterSearchTermQueue()
	:filepath()
{
}

TwitterSearchTermQueue::~TwitterSearchTermQueue() {
	save();
	vector<TwitterSearchTerm*>::iterator it = terms.begin();
	while(it != terms.end()) {
		delete (*it);
		it = terms.erase(it);
	}
}

void TwitterSearchTermQueue::setup(const string& filePath) {
	filepath = filePath;
}


bool TwitterSearchTermQueue::save() {
	std::ofstream ofs(filepath.c_str(),std::ios::binary);
	if(!ofs.is_open()) {
		printf("Cannot open: %s\n", filepath.c_str());
		return false;
	}
	
	// we store only unused items.
	vector<TwitterSearchTerm*> unused;
	getUnusedSearchTerms(unused);

	// check how many items 
	size_t size = unused.size();
	ofs.write((const char*)&size, sizeof(size_t));
	
	for(int i = 0; i < size; ++i) {
		TwitterSearchTerm& term = *unused[i];
		if(term.is_used) {
			continue;
		}
		
		// user
		size_t ulen = term.user.length() + 1;
		ofs.write((const char*)&ulen, sizeof(size_t));
		ofs.write((const char*)term.user.c_str(), ulen);
		
		// search term
		ulen = term.search_term.length() + 1;
		ofs.write((const char*)&ulen, sizeof(size_t));
		ofs.write((const char*)term.search_term.c_str(), ulen);
	
		// is used
		short is_used = term.is_used;
		ofs.write((const char*)&is_used, sizeof(short));	
	}

	ofs.close();
	return true;
}

bool TwitterSearchTermQueue::load() {
	std::ifstream ifs(filepath.c_str(), std::ios::binary);
	if(!ifs.is_open()) {
		printf("Cannot open %s for reading.\n", filepath.c_str());
		return false;
	}
	
	// clear current terms.
	terms.clear();
	
	size_t len = 0;
	size_t size = 0;
	ifs.read((char*)&size, sizeof(size_t));

	for(int i = 0; i < size; ++i) {

		// user
		ifs.read((char*)&len, sizeof(size_t));
		char* user = new char[len];
		ifs.read(user, len);
		
		// search term
		ifs.read((char*)&len, sizeof(size_t));
		char* search_term = new char[len];
		ifs.read(search_term, len);
		
		// is used
		short is_used;
		ifs.read((char*)&is_used, sizeof(short));
		
		TwitterSearchTerm* term = new TwitterSearchTerm(user, search_term, is_used);
		terms.push_back(term);
		term->print();

		delete[] user;
		delete[] search_term;
		
	}
	ifs.close();
	return true;
}


// Add a new search term to the queue; only when it's not yet in there.
bool TwitterSearchTermQueue::addSearchTerm(const string& user, const string& searchTerm) {
	vector<TwitterSearchTerm*>::iterator it = std::find_if(
		 terms.begin()
		,terms.end()
		,TwitterSearchTermCompare(user, searchTerm)
	);
	
	if(it != terms.end()) {
		printf("Search term for user: %s and term: %s already added, %zu items.\n", user.c_str(), searchTerm.c_str(), terms.size());
		return false;
	}
	
	TwitterSearchTerm* st = new TwitterSearchTerm(user, searchTerm, false);
	terms.push_back(st);
	return true;
	
}

bool TwitterSearchTermQueue::getUnusedSearchTerms(vector<TwitterSearchTerm*>& result) {
	if(terms.size() == 0) {
		return false;
	}
	for(int i = 0; i < terms.size(); ++i) {
		if(!terms[i]->is_used) {
			result.push_back(terms[i]);
		}
	}	
	return true;
}

// Get the search term for the given user and set it to used.
bool TwitterSearchTermQueue::setSearchTermAsUsed(const string& user, const string& searchTerm) {
	vector<TwitterSearchTerm*>::iterator it = std::find_if(
		 terms.begin()
		,terms.end()
		,TwitterSearchTermCompare(user, searchTerm)
	);
	if(it == terms.end()) {
		printf("Search term for user: %s  and term: %s not found in queue.\n", user.c_str(), searchTerm.c_str());
		return false;
	}	
	(*it)->is_used = true;
	return true;
}

}
