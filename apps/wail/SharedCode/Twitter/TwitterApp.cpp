#include "TwitterApp.h"
#include "ofxXmlSettings.h"

ofEvent<TwitterAppEvent> twitter_app_dispatcher;

// Init
// -------------------------------------

TwitterApp::TwitterApp()
	:stream(twitter)
	,twitter_listener(*this)
	,uploader(twitter)
{

}

TwitterApp::~TwitterApp() {
}

void TwitterApp::init(int oscPort) {
	initDB();
	initTwitter();
	initOSC(oscPort);
	uploader.startThread();
}

void TwitterApp::initTwitter() {
	// @todo set to correct dewarshub consumer key + secret
	twitter.setConsumerKey("kyw8bCAWKbkP6e1HMMdAvw");
	twitter.setConsumerSecret("PwVuyjLeUdVZbi4ER6yRAo0byF55AIureauV6UhLRw");
	
	//string token_file = ofToDataPath("twitter_roxlu.txt", true);
	string token_file = ofToDataPath("twitter_roxlutest.txt", true);
	if(!twitter.loadTokens(token_file)) {
        string auth_url;
        twitter.requestToken(auth_url);
        twitter.handlePin(auth_url);
        twitter.accessToken();
        twitter.saveTokens(token_file);
	}
}

void TwitterApp::initOSC(int port) {
	osc_receiver.setup(port);
	osc_receiver.addListener(this);
}

void TwitterApp::initDB() {
	if(!mysql.connect("localhost" , "dewarscube_admin", "dewarscube_admin", "dewarscube_admin")) {
		exit(0);
	}
	
	if(!db.open("twitter.db")) {
		printf("Error: Cannot open twitter db.\n");
	}
	
	if(!db.createTables()) {
		printf("Error: Cannot create database.\n");
	}

	search_queue.setup(ofToDataPath("twitter_search_terms.bin",true));
	search_queue.load();

	reloadHashTags();	
	reloadBadWords();	
}

// Events
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
				 
// Bad words & hash tags				 
// -------------------------------------				 
bool TwitterApp::containsBadWord(const string& text) {
	return bad_words.containsBadWord(text);
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
	TwitterAppEvent ev(tweet, term);
	ofNotifyEvent(twitter_app_dispatcher, ev);
}

void TwitterApp::update() {	
	if(stream.isConnected()) {
		stream.update();
	}
	osc_receiver.update();
}

// get the list of people to follow, separated by comma
bool TwitterApp::getFollowers(vector<string>& result) {
	return db.getFollowers(result);
}