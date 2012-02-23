#include "TwitterApp.h"
#include "ofxXmlSettings.h"
#include "ofxWWTweetParticleManager.h"

ofEvent<TwitterAppEvent> twitter_app_dispatcher;

// Init
// -------------------------------------
TwitterApp::TwitterApp(ofxWWTweetParticleManager& manager)
	:stream(twitter)
	,uploader(*this)
	,image_writer(*this)
	,initialized(false)	
	,manager(manager)
{

}

TwitterApp::~TwitterApp() {
	onTwitterStreamDisconnected();
}

void TwitterApp::init(int oscPort) {
	initDB();
	initTwitter();
	initOSC(oscPort);
	initStoredSearchTerms();
	image_writer.startThread(true, false);
	initialized = true;
}

void TwitterApp::initTwitter() {
	// TODO make sure dewarshub key is set
	twitter.setConsumerKey("kyw8bCAWKbkP6e1HMMdAvw");
	twitter.setConsumerSecret("PwVuyjLeUdVZbi4ER6yRAo0byF55AIureauV6UhLRw");
	
	string token_file = ofToDataPath("twitter_dewarshub.txt", true);	
	if(!twitter.loadTokens(token_file)) {
        string auth_url;
        twitter.requestToken(auth_url);
        twitter.handlePin(auth_url);
        twitter.accessToken();
        twitter.saveTokens(token_file);
	}
	
	// We listen to "connection" events of the stream.
	stream.addEventListener(this);
	
	mentions.setup(twitter.getConsumerKey(), twitter.getConsumerSecret(), token_file);
	mentions.startThread(true,false);
	
	uploader.setup(twitter.getConsumerKey(), twitter.getConsumerSecret(), token_file);
	uploader.startThread(true, false);
	
	//removeTweetsFromConnectedAccount();
}

// removes 20 tweets per times!
void TwitterApp::removeTweetsFromConnectedAccount() {
	twitter.statusesUserTimeline(50);
	vector<rtt::Tweet> result;
	twitter.getJSON().parseStatusArray(twitter.getResponse(), result);
	for(int i = 0; i < result.size(); ++i) {
		rtt::Tweet& tweet = result[i];
		printf("> (%d) - %s %s\n",i, tweet.getText().c_str(), tweet.getTweetID().c_str());
		twitter.statusesDestroy(tweet.getTweetID());
	}
}

void TwitterApp::initOSC(int port) {
	osc_receiver.setup(port);
	osc_receiver.addListener(this);
}

void TwitterApp::initDB() {
	//grant all on dewarscube_admin.* to dewarscube_admin@"%" identified by "dewarscube_admin"
	if(!mysql.connect("localhost" , "dewarshub_admin", "dewarshub_admin", "dewarshub_admin", "/Applications/MAMP/tmp/mysql/mysql.sock")) {
	//if(!mysql.connect("localhost" , "dewarshub_admin", "dewarshub_admin", "dewarshub_admin", "/tmp/mysql.sock")) {
		exit(0);
	}
	
	db_thread.startThread(true, false);
		
	reloadHashTags();	
	reloadBadWords();	
}

// load search terms which are saved on disk.
void TwitterApp::initStoredSearchTerms() {
	search_queue.setup(ofToDataPath("twitter_search_terms.bin",true));
}

// OSC
// -------------------------------------

// OSC event: bad words handling
void TwitterApp::onUpdateBadWordList() {
	reloadBadWords();
}

// Reloads the badwords from MySQL DB.
bool TwitterApp::reloadBadWords() {
	vector<string> w;
	if(!mysql.getBadWords(w)) {
		return false;
	}
	bad_words.setBadWords(w);
	return true;
}

// only used to make testing everything a bit more easy.
void TwitterApp::simulateSearch(const string& term) {
	printf("> simulate search.\n");
	rtt::Tweet tweet;
	tweet.setScreenName("roxlu");
	tweet.setText("@dewarshub " +term);
	vector<roxlu::twitter::IEventListener*>& listeners = twitter.getEventListeners();
	for(int i = 0; i < listeners.size(); ++i) {
		listeners[i]->onStatusUpdate(tweet);
	}
}	

void TwitterApp::removeTweet(uint32_t id) {
	ofxWWTweetParticle tweet;
	if(manager.getTweetWithDeleteID(id, tweet)) {
		db_thread.deleteTweetByTweetID(tweet.tweet.getTweetID());
		manager.removeTweetWithDeleteID(id);
	}
}
				 			 
// Bad words & hash tags				 
// -------------------------------------				 
bool TwitterApp::containsBadWord(const string& text, string& foundWord) {
	return bad_words.containsBadWord(text, foundWord);
}

// OSC event: reload hashtags
void TwitterApp::onUpdateHashTags() {
	reloadHashTags();
	if(stream.isConnected()) {
		stream.disconnect();
	}
	connect();
}

// Update the stream connection with new tags to track.
void TwitterApp::reloadHashTags() {
	stream.clearTrackList();
	vector<string> tags;
	if(mysql.getTrackList(tags)) {
		vector<string>::iterator it = tags.begin();
		while(it != tags.end()) {
			stream.track((*it));
			++it;
		}
	}
}


// Twitter
// -------------------------------------
bool TwitterApp::connect(){
	if(!stream.connect(URL_STREAM_USER)) {
		printf("Error: cannot connect to user stream.\n");
		return false;
	}
	return true;
}

void TwitterApp::addCustomStreamListener(rt::IEventListener& listener){
	twitter.addEventListener(listener);
}

void TwitterApp::update() {	
	if(!initialized) {
		printf("TwitterApp: make sure to call init() first!\n");
		exit(0);
	}
	
	if(stream.isConnected()) {
		stream.update();
	}
	
	osc_receiver.update();
	
	// Check for new search terms from the thread which poll mentions
	vector<TwitterMentionSearchTerm> new_search_terms;
	if(mentions.getSearchTerms(new_search_terms)) {
		vector<TwitterMentionSearchTerm>::iterator it = new_search_terms.begin();
		while(it != new_search_terms.end()) {	
			TwitterMentionSearchTerm& st = *it;
			
			// check if the search term or username contains a bad word.
			string found_badword;
			if(containsBadWord(st.search_term, found_badword)) {
				printf("[search with bad word] %s %s\n", st.search_term.c_str(), found_badword.c_str());
				++it;
				continue;
			}
			
			found_badword.clear();
			if(containsBadWord(st.tweet.getScreenName(), found_badword)) {
				printf("[search user with bad word] %s %s\n", st.tweet.getScreenName().c_str(), found_badword.c_str());
				++it;
				continue;
			}
			
			onNewSearchTerm(st.tweet, st.search_term, st.is_old);
			
			++it;
		}
	}
}

void TwitterApp::onNewSearchTerm(rtt::Tweet tweet, const string& term, bool isUsed) {
	
	// When we added a new search term to the queue, pass it through!	
	if(search_queue.addSearchTerm(tweet.getScreenName(), term)) {
		if(isUsed) {
			setSearchTermAsUsed(tweet.getScreenName(), term);
		}
		TwitterAppEvent ev(tweet, term, isUsed);
		ofNotifyEvent(twitter_app_dispatcher, ev);
	}
}


// TODO: remove this; not used in Wailwall
// get the list of people to follow, separated by comma
bool TwitterApp::getFollowers(vector<string>& result) {
	// TODO: fix db
//	return db.getFollowers(result);
}

void TwitterApp::onTwitterStreamDisconnected() {
	mysql.setSetting("twitter_connected", "n");
}

void TwitterApp::onTwitterStreamConnected() {
	mysql.setSetting("twitter_connected", "y");
}

void TwitterApp::executeSearchTest() {
	db_thread.getTweetsWithSearchTerm("love", 100);
}