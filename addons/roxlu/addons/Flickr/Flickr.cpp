#include "Flickr.h"
#include <iostream>

namespace roxlu {

Flickr::Flickr(string key, string secret) 
	:key(key)
	,secret(secret)
{
	oauth.setConsumerKey(key);
	oauth.setConsumerSecret(secret);
}

Flickr::Flickr(const Flickr& other) 
	:key(other.key)
	,secret(other.secret)
{
	oauth = other.oauth;
	printf("copy c'tor\n");
}

Flickr& Flickr::operator=(const Flickr& other) {
	if(this == &other) {
		return *this;
	}
	printf("assignment operator\n");
	key = other.key;
	secret = other.secret;
	oauth = other.oauth;
	return *this;
}

Flickr::~Flickr() {
}

// AUTHORIZATION
// -----------------------------------------------------------------------------
bool Flickr::authorize() {
	// step 1: request a token so we can sign the request for the access token.
	roxlu::curl::Request req = oauth.getRequestTokenRequest(URL_FLICKR_REQUEST_TOKEN,"oob", OAUTH_METHOD_GET);
	string response;
	if(!req.doGet(flickr_curl, response)) {
		printf("Error: cannot get the request token.\n");
		return false;
	}

	// extract token key and secret
	string token_key;
	string token_secret;
	if(!roxlu::curl::oauth::Utils::extractTokenAndSecret(response, token_key, token_secret)) {
		printf("Error: cannot extract key and secret from request token response.\n");
		return false;
	}
	oauth.setTokenKey(token_key);
	oauth.setTokenSecret(token_secret);
		
	// step 2: we let the user authorize the request token via a browser.
	string auth_url = URL_FLICKR_AUTH_URL +token_key +"&perms=write";
	printf("Please visit this url to authorize this application: \n");
	printf("URL: %s\n", auth_url.c_str());
	printf("Enter the code here:\n");
	string pin;
	getline(std::cin, pin);
	oauth.setVerifier(pin);

	printf("---------------------------------\n");	
	printf("Using code: '%s'\n", pin.c_str());
	
	// step 3: using authorized request get the access token.
	roxlu::curl::Request req2 = oauth.getAccessTokenRequest(URL_FLICKR_ACCESS_TOKEN, OAUTH_METHOD_GET);
	if(!req2.doGet(flickr_curl, response)) {
		printf("Error: cannot request access token.\n");
		return false;
	}

	if(!roxlu::curl::oauth::Utils::extractTokenAndSecret(response, token_key, token_secret)) {	
		printf("Error: cannot retrieve access key and secret.\n");
		return false;
	}
	
	// Last step is storing the authorized access token and key.
	oauth.setTokenKey(token_key);
	oauth.setTokenSecret(token_secret);

	return true;
}

bool Flickr::loadTokens(const string& filePath) {
	std::ifstream ifs(filePath.c_str());
	if(!ifs.is_open()) {
		return false;
	}
	string token_key, token_secret;
	std::getline(ifs, token_key);
	std::getline(ifs, token_secret);
	oauth.setTokenKey(token_key);
	oauth.setTokenSecret(token_secret);
	return true;
}

bool Flickr::saveTokens(const string& filePath) {
	std::ofstream of(filePath.c_str());
	if(!of.is_open()) {
		printf("Error: cannot open: '%s'.\n", filePath.c_str());
		return false;
	}
	of << oauth.getTokenKey() << std::endl;
	of << oauth.getTokenSecret() << std::endl;
	of.close();
	return true;
}

bool Flickr::removeTokens(const string& filePath) {
	std::remove(filePath.c_str());
	return true;
}

void Flickr::addDefaultParams(rcp::Collection& params) {
	params.addString("format", "json");
	params.addString("nojsoncallback", "1");
}


// API
// -----------------------------------------------------------------------------
bool Flickr::testLogin() {
	rcp::Collection col;
	col["method"] = "flickr.test.login";
	addDefaultParams(col);
	return doGet(FLICKR_URL_TEST_LOGIN, &col);
}

bool Flickr::upload(const string& file) {
	rcp::Collection col;
	col.addFile("photo", file);
	addDefaultParams(col);
	return doPost("http://api.flickr.com/services/upload/", true, &col);	
}


} // roxlu