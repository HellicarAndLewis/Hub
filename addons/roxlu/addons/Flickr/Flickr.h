#ifndef ROXLU_FLICKRH
#define ROXLU_FLICKRH 

#define WARN_UNUSED __attribute__((warn_unused_result))


#include "../Curl/parameter/Collection.h"
#include "../Curl/parameter/Parameter.h"
#include "../Curl/Request.h"
#include "../Curl/Curl.h"
#include "../Curl/oauth/Utils.h"
#include "../Curl/oauth/oAuth.h"

/*
#include "Collection.h"
#include "Parameter.h"
#include "Request.h"
#include "Curl.h"
#include "Utils.h"
#include "oAuth.h"
*/

#include <fstream>
#include <string>

using std::string;

namespace rc = roxlu::curl;
namespace rcp = roxlu::curl::parameter;
namespace rco = roxlu::curl::oauth;


// http://www.flickr.com/auth-72157629199309695
namespace roxlu {

const string URL_FLICKR_REQUEST_TOKEN = "http://www.flickr.com/services/oauth/request_token";
const string URL_FLICKR_AUTH_URL = "http://www.flickr.com/services/oauth/authorize?oauth_token=";
const string URL_FLICKR_ACCESS_TOKEN = "http://www.flickr.com/services/oauth/access_token";
const string FLICKR_URL_TEST_LOGIN = "http://api.flickr.com/services/rest";

class Flickr {
public:
	Flickr(string key, string secret);
	Flickr(const Flickr& other);
	Flickr& operator=(const Flickr& other);
	~Flickr();
	
	bool loadTokens(const string& filePath);
	bool saveTokens(const string& filePath);
	bool removeTokens(const string& filePath);
	
	string& getResponse();

	bool authorize();
	bool testLogin();
	bool upload(const string& file);
	
	bool doGet(const string& url, rcp::Collection *defaultParams, rcp::Collection* extraParams = NULL);
	bool doPost(const string& url, bool multipart, const rcp::Collection* params, rcp::Collection* extraParams = NULL);
	void addDefaultParams(rcp::Collection& params);
	
private:
	void reset();
	string response;
	string key;
	string secret;		
	roxlu::curl::oauth::oAuth oauth;
	roxlu::curl::Curl flickr_curl;
};

inline void Flickr::reset() {
	response.clear();
}

inline string& Flickr::getResponse() {
	return response;
}

// GET
// ------------------------
inline bool Flickr::doGet(
	 const string& url
	,rcp::Collection* defaultParams
	,rcp::Collection* extraParams
) 
{
	reset();
	rc::Request req(url);
	req.isGet(true);
	
	if(defaultParams != NULL) {
		req.getParams() += *defaultParams;
	}
		
	if(extraParams != NULL) {
		req.getParams() += *extraParams;
	}
	
	
	oauth.authorize(req, OAUTH_METHOD_GET);
	return req.doGet(flickr_curl, response);
}



// POST
// ------------------------
inline bool Flickr::doPost(
	 const string& url
	,bool multipart
	,const rcp::Collection* params
	,rcp::Collection* extraParams
)
{	
	reset();
	rc::Request req(url);
	
	if(params != NULL) {
		req.addParams(*params);
	}
	
	if(extraParams != NULL) {
		req.getParams() += *extraParams;
	}
	req.isPost(true);
	oauth.authorize(req, OAUTH_METHOD_HEADER);
	return req.doPost(flickr_curl, response, multipart);
}


} // roxlu
#endif