#pragma once

#include "ofMain.h"
#include "Twitter.h"

// example listener:
class TwitterListener : public rt::IEventListener {
public:

	virtual void onStatusUpdate(const rtt::Tweet& tweet) {
		printf("> %s\n", tweet.getText().c_str());
	}
	
	virtual void onStatusDestroy(const rtt::StatusDestroy& destroy) {
	}
	
	virtual void onStreamEvent(const rtt::StreamEvent& event) {

	}
};


class testApp : public ofBaseApp{

	public:
		testApp();
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		rt::Twitter twitter;
		rt::Stream stream;		
		TwitterListener listener;
};
