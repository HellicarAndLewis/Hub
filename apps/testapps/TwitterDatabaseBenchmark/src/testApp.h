#pragma once

#include "ofMain.h"
#include "Database.h"
#include "Twitter.h"

using namespace roxlu;
namespace rtt = roxlu::twitter::type;

class testApp : public ofBaseApp{

	public:
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
		
		roxlu::Database db;
		void setWords();
		void createTweets(int howMany, vector<rtt::Tweet>& result);
		
		// Using one table to store tweet-tags info
		void createTablesForTest1();
		void doTest1();
		void findTest1(vector<rtt::Tweet>& result);

		// Using a *-* table.
		void createTablesForTest2();
		void doTest2();
		
		vector<string> words;

};
