#ifndef TWITTER_APPH
#define TWITTER_APPH

#include <vector>
#include "ofMain.h"
#include "Twitter.h"
#include "TwitterDB.h"
#include "TwitterPhotoUploader.h"
#include "TwitterBadWords.h"
#include "IEventListener.h"
#include "TwitterEventListener.h"
#include "TwitterOSCReceiver.h"


namespace rtt = roxlu::twitter::type;
namespace rt = roxlu::twitter;
using namespace roxlu;
using std::vector;

/**
 *
 * Event object which is passed through to objects which added themself
 * as a event listener. We dispatch an event when a new search term is 
 * found.
 *
 */
class TwitterAppEvent { 
public:
	TwitterAppEvent(rtt::Tweet tweet,  string searchTerm)
		:tweet(tweet)
		,search_term(searchTerm)
	{
	}
	
	rtt::Tweet tweet;
	string search_term;
};

extern ofEvent<TwitterAppEvent> twitter_app_dispatcher;


/**
 *
 * This is the public interface for all twitter related actions. All other 
 * parts of "WailWall" will be using this class to interface with all separate
 * features like database, bad words, streaming connection, photo uploading
 * etc...
 *
 */ 
class TwitterApp : public TwitterOSCReceiverListener {

public:

	TwitterApp();
	~TwitterApp();
	
	//bool init();
	bool initDB();
	void track(string trackingString);
	bool connect();
	
	// bad words (o_O)
	bool reloadBadWords();
	bool containsBadWord(const string& text);
	
	// hashtags.
	void reloadHashTags();
	
	
	void update();	
	
	TwitterDB& getDB();
	
	//single shot to notify a load of fake search terms for testing
	void populateFakeSearchTerms(vector<string> fakeTerms);

	bool getFollowers(vector<string>& result);
	bool getTweetsWithTag(const string& tag, int howMany, vector<rtt::Tweet>& result);
	bool getTweetsNewerThan(int age, int howMany, vector<rtt::Tweet>& result);
	bool getTweetsWithSearchTerm(const string& q, int youngerThan, int howMany, vector<rtt::Tweet>& result);
	bool getFakeTweetsWithSearchTerm(vector<rtt::Tweet>& result);
	void onNewSearchTerm(rtt::Tweet tweet, const string& term);
	void uploadScreenshot(const string& filePath, const string& username, const string& message);
	void addDefaultListener();
	void addCustomListener(rt::IEventListener& listener);
	
	// event listener
	template <typename ArgumentsType, class ListenerClass>
	static void addListener(
			ListenerClass* listener
			,void (ListenerClass::*listenerMethod)(ArgumentsType&))
	{
		ofAddListener(twitter_app_dispatcher, listener, listenerMethod);
	}
	
	// implementing TwitterOSCReceiverListener
	virtual void onUpdateBadWordList();
	virtual void onUpdateHashTags();
	
private:
	rt::Twitter twitter;
	rt::Stream	stream;
	TwitterDB db;
	TwitterPhotoUploader uploader;
	TwitterEventListener twitter_listener;
	TwitterBadWords bad_words;
	TwitterOSCReceiver osc_receiver;
	//vector<string> hashtags; // what hashtags to follow?
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

inline bool TwitterApp::getTweetsWithSearchTerm(const string& q, int youngerThan, int howMany, vector<rtt::Tweet>& result) {
	return db.getTweetsWithSearchTerm(q, howMany, youngerThan, result);
}

inline void TwitterApp::uploadScreenshot(const string& filePath, const string& username, const string& message) {
	uploader.addFile(filePath, username, message);
}

#endif