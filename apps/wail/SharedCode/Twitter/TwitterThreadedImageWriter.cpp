#include "TwitterThreadedImageWriter.h"

namespace roxlu {

TwitterThreadedImageWriter::TwitterThreadedImageWriter() {
}

TwitterThreadedImageWriter::~TwitterThreadedImageWriter() {
}

void TwitterThreadedImageWriter::addPixels(const string& filePath, const string& user, unsigned char* pixels, int w, int h) {
	// copy the image data.
	int bpp = 3;
	unsigned char* copy = new unsigned char[w*h*bpp];
	memcpy(copy, pixels, (w*h*bpp)*sizeof(unsigned char));
	
	// add to queue.
	TwitterThreadedImageWriteData data = {filePath, user, copy, w, h};
	lock();
		queue.push_back(data);
	unlock();

}

void TwitterThreadedImageWriter::threadedFunction() {
	size_t num = 0;
	while(true) {
		
		num = queue.size();
		if(num == 0) {
			sleep(1); 
			continue;
		}
		// get new pixels from the queue and remove it.
		lock();
			deque<TwitterThreadedImageWriteData>::iterator it = queue.begin();
			TwitterThreadedImageWriteData data = *it;
			queue.erase(it);
		unlock();
	
		// write it
		ofPixels pix;
		pix.setFromPixels(data.pixels, data.w, data.h, OF_IMAGE_COLOR);
		ofImage img;
		img.setUseTexture(false);
		img.setFromPixels(pix);
		img.saveImage(data.filepath);
		delete[] data.pixels;
		printf("-------------- Writing image for: %s\n", data.user.c_str());
		
	}
}

} // roxlu