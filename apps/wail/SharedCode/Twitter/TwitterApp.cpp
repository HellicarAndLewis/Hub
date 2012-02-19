#include "TwitterApp.h"
#include "ofxXmlSettings.h"

ofEvent<TwitterAppEvent> twitter_app_dispatcher;

// Init
// -------------------------------------

TwitterApp::TwitterApp()
	:stream(twitter)
	,twitter_listener(*this)
	,uploader(*this)
	,image_writer(*this)
	,initialized(false)	
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
	uploader.startThread(true, false);
	image_writer.startThread(true, false);
	
	initialized = true;
}

void TwitterApp::initTwitter() {
	// @todo set to correct dewarshub consumer key + secret
	twitter.setConsumerKey("kyw8bCAWKbkP6e1HMMdAvw");
	twitter.setConsumerSecret("PwVuyjLeUdVZbi4ER6yRAo0byF55AIureauV6UhLRw");
	
	//string token_file = ofToDataPath("twitter_roxlu.txt", true);
	//string token_file = ofToDataPath("twitter_roxlutest.txt", true);
	string token_file = ofToDataPath("twitter_dewarshub.txt", true);
	//string token_file = ofToDataPath("twitter.txt", true);
	if(!twitter.loadTokens(token_file)) {
        string auth_url;
        twitter.requestToken(auth_url);
        twitter.handlePin(auth_url);
        twitter.accessToken();
        twitter.saveTokens(token_file);
	}
	
	stream.addEventListener(this);
	//removeTweetsFromConnectedAccount();
}

// removes 20 tweets per times!
void TwitterApp::removeTweetsFromConnectedAccount() {
	twitter.statusesUserTimeline();
	vector<rtt::Tweet> result;
	twitter.getJSON().parseStatusArray(twitter.getResponse(), result);
	for(int i = 0; i < result.size(); ++i) {
		rtt::Tweet& tweet = result[i];
		printf("> %s %s\n", tweet.getText().c_str(), tweet.getTweetID().c_str());
		twitter.statusesDestroy(tweet.getTweetID());
	}
}

void TwitterApp::initOSC(int port) {
	osc_receiver.setup(port);
	osc_receiver.addListener(this);
}

void TwitterApp::initDB() {
	//grant all on dewarscube_admin.* to dewarscube_admin@"%" identified by "dewarscube_admin"
	if(!mysql.connect("localhost" , "dewarshub_admin", "dewarshub_admin", "dewarshub_admin")) {
	//if(!mysql.connect("dewarshub.demo.apollomedia.nl" , "dewarshub_admin", "dewarshub_admin", "dewarshub_admin")) {
		exit(0);
	}
	
	db_thread.startThread(true, false);
		
	reloadHashTags();	
	reloadBadWords();	
}

// load search terms which are saved on disk.
void TwitterApp::initStoredSearchTerms() {
	search_queue.setup(ofToDataPath("twitter_search_terms.bin",true));
	search_queue.load();
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
	twitter_listener.onStatusUpdate(tweet);
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

void TwitterApp::addDefaultListener(){
	addCustomListener(twitter_listener);
}

void TwitterApp::addCustomListener(rt::IEventListener& listener){
	twitter.addEventListener(listener);
}

void TwitterApp::onNewSearchTerm(rtt::Tweet tweet, const string& term) {
	// When we added a new search term to the queue, pass it through!
	if(search_queue.addSearchTerm(tweet.getScreenName(), term)) {
		TwitterAppEvent ev(tweet, term);
		ofNotifyEvent(twitter_app_dispatcher, ev);
	}
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

// Just for testing.
void TwitterApp::executeSearchTest() {
	int start = ofGetElapsedTimeMillis();
	vector<rtt::Tweet> found;
	db_thread.getTweetsWithSearchTerm("love", 99999, 100, found);
	int end = ofGetElapsedTimeMillis();
	int diff = end - start;
	printf("Testing search, found %zu item in %d millis\n", found.size(), diff );
}