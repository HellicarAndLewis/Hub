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

	size_t size = terms.size();
	ofs.write((const char*)&size, sizeof(size_t));
	
	for(int i = 0; i < size; ++i) {
		TwitterSearchTerm& term = *terms[i];
		
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
		term->print();
		terms.push_back(term);

		delete[] user;
		delete[] search_term;
		
	}
	ifs.close();
	return true;
}

void TwitterSearchTermQueue::addSearchTerm(const string& user, const string& searchTerm) {
	TwitterSearchTerm* st = new TwitterSearchTerm(user, searchTerm, false);
	terms.push_back(st);
	
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

}
