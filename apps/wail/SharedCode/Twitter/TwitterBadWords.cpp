#include "TwitterBadWords.h"
namespace roxlu {

bool TwitterBadWords::reloadWordsFile(const string& filePath) {
	bad_words.clear();
	
	std::ifstream ifs(filePath.c_str());
	if(!ifs.is_open()) {
		printf("Error: cannot open '%s'\n", filePath.c_str());
		exit(0);
		return false;
	}	
	
	string line;
	while(std::getline(ifs,line)) {
		if(line.length()) {
			//bad_words.push_back("(^|\\s)" +line +"(\\s|$)");
			bad_words.push_back(line);
		}
	}
	ifs.close();
	return true;
}

void TwitterBadWords::cleanup(string& text, const string& replacement) {
	pcrecpp::RE_Options re_opts;
	re_opts.set_caseless(true).set_extended(false).set_multiline(true);	
	vector<string>::iterator it = bad_words.begin();
	while(it != bad_words.end()) {
		pcrecpp::RE(*it, re_opts).Replace(replacement, &text);
		++it;
	}	
}


bool TwitterBadWords::containsBadWord(const string& text) {
	pcrecpp::RE_Options re_opts;
	re_opts.set_caseless(true)
			.set_extended(false)
			.set_multiline(true);	
	vector<string>::iterator it = bad_words.begin();
	while(it != bad_words.end()) {
		if(pcrecpp::RE((*it), re_opts).PartialMatch(text)) {
			//printf(" > bad word: %s < ", (*it).c_str());
			return true;
		}
		++it;
	}
	return false;
}

} // roxlu