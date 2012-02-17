#ifndef ROLXU_TWITTER_MYSQLH
#define ROLXU_TWITTER_MYSQLH

#include <string>
#include <vector>
#include "mysql.h"

using std::string;
using std::vector;

namespace roxlu {

class TwitterMySQL {

public:

	TwitterMySQL();
	~TwitterMySQL();
	bool connect(const string& server, const string& db, const string& user, const string& pass);
	bool getBadWords(vector<string>& result);
	bool getTrackList(vector<string>& result);
	bool getSetting(const string& name, string& result);
	bool setSetting(const string& name, const string& value);
	
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