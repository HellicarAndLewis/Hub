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
#include "TwitterDBThread.h"
#include "TwitterPhotoUploader.h"
#include "TwitterBadWords.h"
#include "TwitterEventListener.h"
#include "TwitterOSCReceiver.h"
#include "TwitterSearchTermQueue.h"
#include "TwitterMySQL.h"
#include "TwitterThreadedImageWriter.h"
#include "IEventListener.h"
#include "IStreamEventListener.h"
#include "TwitterMentionsThread.h"


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
	TwitterAppEvent(rtt::Tweet tweet,  string searchTerm, bool isUsed)
		:tweet(tweet)
		,search_term(searchTerm)
		,is_used(isUsed)
	{
	}
	
	rtt::Tweet tweet;
	string search_term;
	bool is_used;
};

extern ofEvent<TwitterAppEvent> twitter_app_dispatcher;



class TwitterApp : 
			 public TwitterOSCReceiverListener
			,public roxlu::twitter::IStreamEventListener 
{

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
	bool getTweetsWithSearchTerm(const string& q, int howMany);
	void onNewSearchTerm(rtt::Tweet tweet, const string& term, bool isUsed = false);
	void uploadScreenshot(const string& filePath, const string& username, const string& message);
	void writeScreenshot(const string& filePath, const string& username, ofPixels pixels);
	
	// db actions
	bool insertTweet(const rtt::Tweet& tweet);
	bool insertSendQueueItem(const string& username, const string& filename, int& newID);
	bool setSendQueueItemAsSend(int queueID);
	bool getNextSendItemFromSendQueue(string& username, string& filename, int& id);
	bool retrieveSearchResultsFromThread(vector<rtt::Tweet>& result);
	void getMoreTweetsMatchingCurrentSearchTerm();
	
	void addCustomStreamListener(rt::IEventListener& listener);
	
	template <typename ArgumentsType, class ListenerClass>
	static void addNewSearchTermListener(
			ListenerClass* listener
			,void (ListenerClass::*listenerMethod)(ArgumentsType&))
	{
		ofAddListener(twitter_app_dispatcher, listener, listenerMethod);
	}
	
	
	// OSC events.
	virtual void onUpdateBadWordList();
	virtual void onUpdateHashTags();
	virtual void simulateSearch(const string& term);
	virtual void removeTweet(uint32_t id) = 0;
	
	void removeTweetsFromConnectedAccount();
	
	rt::Twitter& getTwitter();
	TwitterThreadedImageWriter& getImageWriter();
	virtual void onTwitterStreamDisconnected();
	virtual void onTwitterStreamConnected();
	
private:
	bool initialized;
	void initDB();
	void initTwitter();
	void initOSC(int port);
	void initStoredSearchTerms();
	void executeSearchTest();
	
	rt::Twitter 				twitter;
	rt::Stream					stream;
	TwitterDBThread				db_thread;
	TwitterPhotoUploader 		uploader;
	TwitterBadWords 			bad_words;
	TwitterOSCReceiver 			osc_receiver;
	TwitterSearchTermQueue 		search_queue;
	TwitterMySQL 				mysql;
	TwitterThreadedImageWriter 	image_writer;
	TwitterMentionsThread		mentions;

};


// DATABASE
// -------------------------------
inline bool TwitterApp::insertTweet(const rtt::Tweet& tweet) {
	return db_thread.insertTweet(tweet);
}

inline bool TwitterApp::getTweetsWithSearchTerm(const string& q, int howMany) {
	return db_thread.getTweetsWithSearchTerm(q, howMany);
}

inline bool TwitterApp::insertSendQueueItem(const string& username, const string& filename, int& newID) {
	return db_thread.insertSendQueueItem(username, filename, newID);
}

inline bool TwitterApp::retrieveSearchResultsFromThread(vector<rtt::Tweet>& result) {
	return db_thread.retrieveSearchResultsFromThread(result);
}

inline void TwitterApp::getMoreTweetsMatchingCurrentSearchTerm() {
	db_thread.getMoreTweetsMatchingCurrentSearchTerm();
}

inline bool TwitterApp::setSendQueueItemAsSend(int queueID) {
	return db_thread.setSendQueueItemAsSend(queueID);
}

inline bool TwitterApp::getNextSendItemFromSendQueue(string& username, string& filename, int& id) {
	return db_thread.getNextSendItemFromSendQueue(username, filename, id);
}

inline bool TwitterApp::getUnusedSearchTerms(vector<TwitterSearchTerm*>& result) {
	return search_queue.getUnusedSearchTerms(result);
}

inline bool TwitterApp::setSearchTermAsUsed(const string& user, const string& term) {
	return search_queue.setSearchTermAsUsed(user, term);
}


inline TwitterThreadedImageWriter& TwitterApp::getImageWriter() {
	return image_writer;
}



inline rt::Twitter& TwitterApp::getTwitter() {
	return twitter;
}
#endif