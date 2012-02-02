#ifndef TWITTER_APPH
#define TWITTER_APPH

#include "ofMain.h"
#include "Twitter.h"
#include "TwitterDB.h"
#include "TwitterEventListener.h"

namespace rtt = roxlu::twitter::type;
namespace rt = roxlu::twitter;
using namespace roxlu;

class TwitterApp {
public:
	TwitterApp();
	~TwitterApp();
	bool init();
	void update();	
	
	TwitterDB& getDB();
	
	bool getFollowers(vector<string>& result);
	bool getTweetsWithTag(const string& tag, int howMany, vector<rtt::Tweet>& result);
	bool getTweetsNewerThan(int age, int howMany, vector<rtt::Tweet>& result);
	bool getTweetsWithSearchTerm(const string& q, vector<rtt::Tweet>& result);

private:
	rt::Twitter twitter;
	rt::Stream	stream;
	TwitterDB db;
	TwitterEventListener twitter_listener;
};

inline TwitterDB& TwitterApp::getDB() {
	return db;
}


inline bool TwitterApp::getTweetsWithTag(const string& tag, int howMany, vector<rtt::Tweet>& result) {
	return db.getTweetsWithTag(tag, howMany, result);
}

inline bool TwitterApp::getTweetsNewerThan(int age, int howMany, vector<rtt::Tweet>& result) {
	return db.getTweetsNewerThan(age, howMany, result);
}

inline bool TwitterApp::getTweetsWithSearchTerm(const string& q, vector<rtt::Tweet>& result) {
	return db.getTweetsWithSearchTerm(q, result);
}

#endif