#include "TwitterOSCReceiver.h"

namespace roxlu {

TwitterOSCReceiver::TwitterOSCReceiver() {
}

void TwitterOSCReceiver::setup(int port) {
	osc.setup(port);
}

void TwitterOSCReceiver::update() {
	ofxOscMessage m;
	while(osc.getNextMessage(&m)) {
		if(m.getAddress() == "/twitter/reload_badwords") {
			for(int i = 0; i < listeners.size(); ++i) {
				listeners[i]->onUpdateBadWordList();
			}
		}
		else if(m.getAddress() == "/twitter/reload_hashtags") {
			for(int i = 0; i < listeners.size(); ++i) {
				listeners[i]->onUpdateHashTags();
			}
		}
		else if(m.getAddress() == "/twitter/simulate_search") {
			string search_term = m.getArgAsString(0);
			
			for(int i = 0; i < listeners.size(); ++i) {
				listeners[i]->simulateSearch(search_term);
			}
		}
		else if(m.getAddress() == "/twitter/remove_tweet") {
			uint32_t id = m.getArgAsInt(0);
			for(int i = 0; i < listeners.size(); ++i) {
				listeners[i]->removeTweet(id);
			}
		}
	}
}

void TwitterOSCReceiver::addListener(TwitterOSCReceiverListener* listener) {
	listeners.push_back(listener);
}

} // roxlu