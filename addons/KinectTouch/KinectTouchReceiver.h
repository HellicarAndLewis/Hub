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
 *  Description: Wraps ofxOsc and our defined protocol to provide a listener pattern
 *               using KinectTouchListener. 
 *
 *               Call setup(<port>), then setListener(<your listener>)
 *               then every frame call update()
 *      
 *               Depends on ofxOsc.
 *				 
 *  KinectTouchReceiver.h, created by Marek Bereza on 30/01/2012.
 */
#include "KinectTouchListener.h"
#include "ofxOsc.h"


class KinectTouchReceiver {

public:
	KinectTouchReceiver();
	
	// call this in your testApp::setup() with the desired OSC port
	void setup(int port);
	
	// add yourself as a listener (implementing the KinectTouchListener interface)
	void setListener(KinectTouchListener *listener);
	
	
	// call this every testApp::update() to receive KinectTouchListner events.
	void update();
	
private:
	ofxOscReceiver osc;
	KinectTouchListener *listener;
	map<int,KinectTouch> blobs;
};


