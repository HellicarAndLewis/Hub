#ifndef TWITTER_APPH
#define TWITTER_APPH

#include "ofMain.h"
#include "Twitter.h"
#include "TwitterDB.h"
#include "TwitterEventListener.h"

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
private:
	rt::Twitter twitter;
	rt::Stream	stream;
	TwitterDB db;
	TwitterEventListener twitter_listener;
};

inline TwitterDB& TwitterApp::getDB() {
	return db;
}
#endif