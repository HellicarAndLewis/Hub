#ifndef ROXLU_OSC_RECEIVER_LISTENERH
#define ROXLU_OSC_RECEIVER_LISTENERH

namespace roxlu {

class TwitterOSCReceiverListener {

public:

	virtual void onUpdateBadWordList() = 0;
	virtual void onUpdateHashTags() = 0;

};

} // roxlu

#endif