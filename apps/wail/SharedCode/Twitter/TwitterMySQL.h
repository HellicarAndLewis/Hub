#ifndef ROLXU_TWITTER_MYSQLH
#define ROLXU_TWITTER_MYSQLH

#include <string>
#include <vector>
#include "mysql.h"
#include "Twitter.h"

using std::string;
using std::vector;

namespace rtt = roxlu::twitter::type;

namespace roxlu {

class TwitterMySQL {

public:

	TwitterMySQL();
	~TwitterMySQL();
	bool connect(const string& server, const string& db, const string& user, const string& pass, const string& unixSocket);
	bool getBadWords(vector<string>& result);
	bool getTrackList(vector<string>& result);
	bool getSetting(const string& name, string& result);
	bool setSetting(const string& name, const string& value);
	
	// TODO: trying to use just mysql w/o sqlite
	bool insertTweet(const rtt::Tweet& tweet);
	
private:
	bool connected;
	string host;
	string username;
	string password;
	string database;
	MYSQL conn;
};

} // roxlu

#endif