/**     ___           ___           ___                         ___           ___     
 *     /__/\         /  /\         /  /\         _____         /  /\         /__/|    
 *    |  |::\       /  /::\       /  /::|       /  /::\       /  /::\       |  |:|    
 *    |  |:|:\     /  /:/\:\     /  /:/:|      /  /:/\:\     /  /:/\:\      |  |:|    
 *  __|__|:|\:\   /  /:/~/::\   /  /:/|:|__   /  /:/~/::\   /  /:/  \:\   __|__|:|    
 * /__/::::| \:\ /__/:/ /:/\:\ /__/:/ |:| /\ /__/:/ /:/\:| /__/:/ \__\:\ /__/::::\____
 * \  \:\~~\__\/ \  \:\/:/__\/ \__\/  |:|/:/ \  \:\/:/~/:/ \  \:\ /  /:/    ~\~~\::::/
 *  \  \:\        \  \::/          |  |:/:/   \  \::/ /:/   \  \:\  /:/      |~~|:|~~ 
 *   \  \:\        \  \:\          |  |::/     \  \:\/:/     \  \:\/:/       |  |:|   
 *    \  \:\        \  \:\         |  |:/       \  \::/       \  \::/        |  |:|   
 *     \__\/         \__\/         |__|/         \__\/         \__\/         |__|/   
 *
 *  Description: 
 *				 
 *  KinectTouchReceiver.h, created by Marek Bereza on 30/01/2012.
 */
#include "KinectTouchListener.h"
#include "ofxOsc.h"

class KinectTouchReceiver {

public:
	KinectTouchReceiver() {
		this->listener = NULL;
	}
	void setup(int port) {
		osc.setup(port);
	}
	void setListener(KinectTouchListener *listener) {
		this->listener = listener;
	}
	
	void update() {
		ofxOscMessage m;
		while(osc.getNextMessage(&m)) {
			// if there's no listener, continue to consume messages, but don't do anything about them
			if(listener==NULL) continue; 
			
			// otherwise deal with message
//			printf("Address: %s\n", m.getAddress().c_str());
			if(m.getAddress()=="/touch/down") {
				int id = m.getArgAsInt32(0);
				ofVec3f p = ofVec3f(m.getArgAsFloat(1), m.getArgAsFloat(2), m.getArgAsFloat(3));
				listener->touchDown(id, p);
			} else if(m.getAddress()=="/touch/moved") {
				int id = m.getArgAsInt32(0);
				ofVec3f p = ofVec3f(m.getArgAsFloat(1), m.getArgAsFloat(2), m.getArgAsFloat(3));
				listener->touchMoved(id, p);
				//printf("%d %f %f %f\n", id, p.x, p.y, p.z);				
			} else if(m.getAddress()=="/touch/up") {
				listener->touchUp(m.getArgAsInt32(0));
			}
		}
	}
	
private:
	ofxOscReceiver osc;
	KinectTouchListener *listener;
};


