#ifndef ROXLU_OSC_RECEIVER_LISTENERH
#define ROXLU_OSC_RECEIVER_LISTENERH

#include <string>
using std::string;

namespace roxlu {

class TwitterOSCReceiverListener {

public:

	virtual void onUpdateBadWordList() = 0;
	virtual void onUpdateHashTags() = 0;
	virtual void simulateSearch(const string& term) = 0;
	virtual void removeTweet(uint32_t id) = 0;
};

} // roxlu

#endif