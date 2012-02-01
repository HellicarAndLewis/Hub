#include "TwitterApp.h"

TwitterApp::TwitterApp()
	:stream(twitter)
	,twitter_listener(*this)
{
}

TwitterApp::~TwitterApp() {
}

bool TwitterApp::init() {
	
	// DATABASE 
	// --------
	if(!db.open("twitter.db")) {
		printf("Error: Cannot open twitter db.\n");
		return false;
	}
	
	if(!db.createTables()) {
		printf("Error: Cannot create database.\n");
		return false;
	}
	
	// TWITTER
	// --------
	twitter.setTwitterUsername("dewarshub");
	twitter.setTwitterPassword("HUB2012hub#");
	twitter.setConsumerKey("5cL1KRDQzcnGo8ZOaAz0g");
	twitter.setConsumerSecret("e4X9dtxkgmpkRlr9arhOfNe7tTezWad2bmCUNvPtBvQ");
	
	string token_file = ofToDataPath("twitter.txt", true);
	if(!twitter.loadTokens(token_file)) {
        string auth_url;
        twitter.requestToken(auth_url);
        twitter.handlePin(auth_url);
        twitter.accessToken();
        twitter.saveTokens(token_file);
	}

	//stream.track("3WordsThatWomenHate");	
	if(!stream.connect(URL_STREAM_USER)) {
		printf("Error: cannot connect to user stream.\n");
		return false;
	}
	
	twitter.addEventListener(twitter_listener);

	return true;
}


void TwitterApp::update() {	
	if(stream.isConnected()) {
		stream.update();
	}
}

// get the list of people to follow, separated by comma
bool TwitterApp::getFollowers(vector<string>& result) {
	return db.getFollowers(result);
}
