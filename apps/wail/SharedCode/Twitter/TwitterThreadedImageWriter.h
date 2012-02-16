#ifndef ROXLU_TWITTER_THREADED_IMAGE_WRITERH
#define ROXLU_TWITTER_THREADED_IMAGE_WRITERH

#include "ofMain.h"
#include <deque>

using std::deque;

namespace roxlu {

struct TwitterThreadedImageWriteData {
	string filepath;
	string user;
	ofPixels pixels;
};

class TwitterThreadedImageWriter : public ofThread {

public:
	TwitterThreadedImageWriter();
	~TwitterThreadedImageWriter();
	
	void addPixels(const string& filePath, const string& username, ofPixels pixels);
	void threadedFunction();

private:

	deque<TwitterThreadedImageWriteData> queue;
};


} // roxlu

#endif