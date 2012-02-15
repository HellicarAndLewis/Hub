
/**
 * 
 * This class is used by the Twitter part of the WailWall application. 
 * It's used to filter out tweet messages which contain a word that's 
 * not allowed to be shown.
 * 
 * Note: not all functions are used in this application.
 * 
 * @date	2012.02.01
 * @author	Diederick Huijbers <diederick@apollomedia.nl>
 *
 */
 
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

	void setBadWords(const vector<string>& words);
	bool reloadWordsFile(const string& filePath);
	void cleanup(string& text, const string& replacement);
	bool containsBadWord(const string& text, string& foundWord);
	
private:

	vector<string> bad_words;
};

} // roxlu
#endif