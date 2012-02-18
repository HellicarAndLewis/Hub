#ifndef ROXLU_TWITTER_THREADED_IMAGE_WRITERH
#define ROXLU_TWITTER_THREADED_IMAGE_WRITERH

#include "ofMain.h"
#include <deque>
#include "Twitter.h"
//#include "TwitterPhotoUploader.h"

using std::deque;

class TwitterApp;

namespace roxlu {

struct TwitterThreadedImageWriteData {
	string filepath;
	string user;
	//ofPixels pixels;
	
	unsigned char* pixels;
	int w;
	int h;
};

class TwitterThreadedImageWriter : public ofThread {

public:
	TwitterThreadedImageWriter(TwitterApp& app);
	~TwitterThreadedImageWriter();
	
	void addPixels(const string& filePath, const string& username, unsigned char* pixels, int w, int h);
	void threadedFunction();
	//void storePixels(unsigned char* pix, int w, int h);
private:
	TwitterApp& app;
	deque<TwitterThreadedImageWriteData> queue;
	//TwitterPhotoUploader uploader;
};


} // roxlu

#endif