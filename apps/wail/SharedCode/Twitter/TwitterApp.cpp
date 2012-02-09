#include "TwitterApp.h"
#include "ofxXmlSettings.h"

ofEvent<TwitterAppEvent> twitter_app_dispatcher;

TwitterApp::TwitterApp()
	:stream(twitter)
	,twitter_listener(*this)
{

}

TwitterApp::~TwitterApp() {
}

bool TwitterApp::initDB(){
	// @todo create a init function
	reloadBadWords();	
	
	// OSC RECEIVER
	// ------------
	osc_receiver.setup(4444);
	osc_receiver.addListener(this);
	
	
	// TWITTER
	// --------
	reloadHashTags();	
	
	// @todo set to correct dewarshub consumer key + secret
	twitter.setConsumerKey("kyw8bCAWKbkP6e1HMMdAvw");
	twitter.setConsumerSecret("PwVuyjLeUdVZbi4ER6yRAo0byF55AIureauV6UhLRw");

	string token_file = ofToDataPath("twitter.txt", true);
	//twitter.removeTokens(token_file);
	if(!twitter.loadTokens(token_file)) {
        string auth_url;
        twitter.requestToken(auth_url);
        twitter.handlePin(auth_url);
        twitter.accessToken();
        twitter.saveTokens(token_file);
	}

	// DATABASE 
	// --------
	if(!db.open("twitter.db")) {
		printf("Error: Cannot open twitter db.\n");
	}
	
	if(!db.createTables()) {
		printf("Error: Cannot create database.\n");
	}
	
	// UPLOADER
	// --------
	uploader.startThread();
	return true;
}

// Bad words handling
// -------------------------------------
void TwitterApp::onUpdateBadWordList() {
	printf("\n\n\n----------------> on update bad wordlist. <---------------------\n\n\n\n");
	reloadBadWords();
}

bool TwitterApp::reloadBadWords() {
	string file = ofToDataPath("badwords.txt", true);
	return bad_words.reloadWordsFile(file);
}
				 
bool TwitterApp::containsBadWord(const string& text) {
	return bad_words.containsBadWord(text);
}


// Reload hashtags
// -------------------------------------
void TwitterApp::onUpdateHashTags() {
	printf("\n\n\n----------------> on hash tags <---------------------\n\n\n\n");
	reloadHashTags();
	if(stream.isConnected()) {
		stream.disconnect();
	}
	connect();
}

void TwitterApp::reloadHashTags() {
	stream.clearTrackList();
	
	string file = ofToDataPath("twitter_hashtags.txt", true);
	ifstream ifs(file.c_str());
	if(!ifs.is_open()) {
		printf("Cannot open hashtag list");
		exit(0);
	}
	
	string line;
	while(std::getline(ifs, line)) {
		printf("line: %s\n", line.c_str());
		if(line.length()) {
			stream.track(line);
		}
	}
	ifs.close();
}


// Twitter
// -------------------------------------
void TwitterApp::track(string trackingString){
	stream.track(trackingString);
}

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

void TwitterApp::populateFakeSearchTerms(vector<string> fakeTerms){
	rtt::Tweet fakeTweek;
	for(int i = 0; i < fakeTerms.size(); i++){
		onNewSearchTerm(fakeTweek, fakeTerms[i]);
	}
}

bool TwitterApp::getFakeTweetsWithSearchTerm(vector<rtt::Tweet>& result){
	
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