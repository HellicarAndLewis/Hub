/**
 *
 * This is the public interface for all twitter related actions. All other 
 * parts of "WailWall" will be using this class to interface with all separate
 * features like database, bad words, streaming connection, photo uploading
 * etc...
 *
 * @date	2012.01.xx
 * @author	Diederick Huijbers <diederick@apollomedia.nl>
 *
 */ 

#ifndef TWITTER_APPH
#define TWITTER_APPH

#include <vector>
#include "ofMain.h"
#include "Twitter.h"
#include "TwitterDB.h"
#include "TwitterPhotoUploader.h"
#include "TwitterBadWords.h"
#include "TwitterEventListener.h"
#include "TwitterOSCReceiver.h"
#include "TwitterSearchTermQueue.h"
#include "TwitterMySQL.h"
#include "TwitterThreadedImageWriter.h"
#include "IEventListener.h"

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

class TwitterApp : public TwitterOSCReceiverListener {

public:

	TwitterApp();
	~TwitterApp();
	
	void init(int oscPort);
	bool connect();
	void update();	
	
	bool reloadBadWords();
	bool containsBadWord(const string& text, string& foundWord);
	void reloadHashTags();

	bool getUnusedSearchTerms(vector<TwitterSearchTerm*>& result);
	bool setSearchTermAsUsed(const string& user, const string& term);
	bool getFollowers(vector<string>& result);
	bool getTweetsWithTag(const string& tag, int howMany, vector<rtt::Tweet>& result);
	bool getTweetsNewerThan(int age, int howMany, vector<rtt::Tweet>& result);
	bool getTweetsWithSearchTerm(const string& q, int youngerThan, int howMany, vector<rtt::Tweet>& result);
	void onNewSearchTerm(rtt::Tweet tweet, const string& term);
	void uploadScreenshot(const string& filePath, const string& username, const string& message);
	void writeScreenshot(const string& filePath, const string& username, ofPixels pixels);
	void addDefaultListener();
	void addCustomListener(rt::IEventListener& listener);
	
	template <typename ArgumentsType, class ListenerClass>
	static void addListener(
			ListenerClass* listener
			,void (ListenerClass::*listenerMethod)(ArgumentsType&))
	{
		ofAddListener(twitter_app_dispatcher, listener, listenerMethod);
	}
	
	// OSC events.
	virtual void onUpdateBadWordList();
	virtual void onUpdateHashTags();
	virtual void simulateSearch(const string& term);
	
	TwitterDB& getDB();	
	
private:
	bool initialized;
	void initDB();
	void initTwitter();
	void initOSC(int port);
	void initStoredSearchTerms();

	rt::Twitter 			twitter;
	rt::Stream				stream;
	TwitterDB 				db;
	TwitterPhotoUploader 	uploader;
	TwitterEventListener 	twitter_listener;
	TwitterBadWords 		bad_words;
	TwitterOSCReceiver 		osc_receiver;
	TwitterSearchTermQueue 	search_queue;
	TwitterMySQL 			mysql;
	TwitterThreadedImageWriter image_writer;

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

inline bool TwitterApp::getUnusedSearchTerms(vector<TwitterSearchTerm*>& result) {
	return search_queue.getUnusedSearchTerms(result);
}

inline bool TwitterApp::setSearchTermAsUsed(const string& user, const string& term) {
	return search_queue.setSearchTermAsUsed(user, term);
}


inline void TwitterApp::writeScreenshot(const string& filePath, const string& user, ofPixels pixels) {
	image_writer.addPixels(filePath, user, pixels);
}
#endif