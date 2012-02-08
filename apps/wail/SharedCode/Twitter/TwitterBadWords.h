#ifndef TWITTER_BAD_WORDSH
#define TWITTER_BAD_WORDSH

#include <string>
#include <vector>
#include <fstream>
#include "pcrecpp.h"

using std::string;
using std::vector;

namespace roxlu {

class TwitterBadWords {
public:
	bool reloadWordsFile(const string& filePath);
	void cleanup(string& text, const string& replacement);
private:
	vector<string> bad_words;
};

} // roxlu
#endif