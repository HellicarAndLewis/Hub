#ifndef ROXLU_TWITTER_OSC_RECEIVERH
#define ROXLU_TWITTER_OSC_RECEIVERH

#include <vector>
#include "ofxOsc.h"
#include "TwitterOSCReceiverListener.h"

using std::vector;

namespace roxlu {

/**
 * Receives message which change the state of the TwitterApp. This class is
 * used so we can interface this through some external GUI and tell the
 * app to reload i.e. the word list, or list with hash tags to follow.
 * 
 */

class TwitterOSCReceiver {

public:

	TwitterOSCReceiver();
	void setup(int port);
	void update();
	void addListener(TwitterOSCReceiverListener* listener);

private:

	ofxOscReceiver osc;
	vector<TwitterOSCReceiverListener*> listeners;
};

} // roxlu

#endif