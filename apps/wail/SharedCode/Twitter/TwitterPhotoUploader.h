#ifndef ROXLU_TWITTER_PHOTO_UPLOADERH
#define ROXLU_TWITTER_PHOTO_UPLOADERH

#include <string>
#include "Curl.h"
#include "Request.h"
#include "ofMain.h"
#include "Twitter.h"
#include "curl.h"
#include "jansson.h"

namespace rc = roxlu::curl;
namespace rcp = roxlu::curl::parameter;
using std::string;

//const string URL_TWITTER_UPLOADER = "http://dewarshub.demo.apollomedia.nl/";
const string URL_TWITTER_UPLOADER = "http://dewarshub.localhost/";

namespace roxlu {

struct UploadFileInfo {
	string file;
	string username;
	string message;
};

class TwitterPhotoUploader : public ofThread {
public:
	TwitterPhotoUploader(rt::Twitter& twitter);
	void addFile(const string& file, const string& username, const string& message);
	void threadedFunction();
		
private:
	void upload(const string& file);
	deque<UploadFileInfo> upload_queue;
	rc::Curl uploader_curl;
	rt::Twitter& twitter;
};

} // roxlu

#endif