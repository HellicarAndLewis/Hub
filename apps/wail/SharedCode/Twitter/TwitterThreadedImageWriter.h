#ifndef ROXLU_TWITTER_THREADED_IMAGE_WRITERH
#define ROXLU_TWITTER_THREADED_IMAGE_WRITERH

#include "ofMain.h"
#include <deque>

using std::deque;

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
	TwitterThreadedImageWriter();
	~TwitterThreadedImageWriter();
	
	void addPixels(const string& filePath, const string& username, unsigned char* pixels, int w, int h);
	void threadedFunction();
	//void storePixels(unsigned char* pix, int w, int h);
private:

	deque<TwitterThreadedImageWriteData> queue;
};


} // roxlu

#endif