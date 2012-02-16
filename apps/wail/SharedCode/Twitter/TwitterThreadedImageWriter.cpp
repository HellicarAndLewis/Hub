#include "TwitterThreadedImageWriter.h"

namespace roxlu {

TwitterThreadedImageWriter::TwitterThreadedImageWriter() {
}

TwitterThreadedImageWriter::~TwitterThreadedImageWriter() {
}

void TwitterThreadedImageWriter::addPixels(const string& filePath, const string& user, ofPixels pixels) {
	TwitterThreadedImageWriteData data = {filePath, user, pixels};
	lock();
		queue.push_back(data);
	unlock();
}

void TwitterThreadedImageWriter::threadedFunction() {
	size_t num = 0;
	while(true) {
		
		num = queue.size();
		if(num == 0) {
			sleep(3); 
			continue;
		}
		
		// get new pixels from the queue and remove it.
		lock();
			deque<TwitterThreadedImageWriteData>::iterator it = queue.begin();
			TwitterThreadedImageWriteData data = *it;
			queue.erase(it);
		unlock();
	
		// write it
		printf("Writing image for: %s\n", data.user.c_str());
		
	}
}

} // roxlu