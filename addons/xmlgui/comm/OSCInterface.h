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
 *  OSCInterface.h, created by Marek Bereza on 13/02/2012.
 */

#pragma once
#include "Container.h"
#include "ofxOsc.h"

namespace xmlgui {
	class OSCInterface {
	public:
		void setup(Container &root, int receiveOscPort, int sendOscPort = 0, string sendOscHost = "localhost");
		Container *root;
		
		// call this every update
		void update();
	private:
		void setupOsc(int receiveOscPort, int sendOscPort = 0, string sendOscHost = "localhost");		
		string OSCInterface::describeGuiAsXml(xmlgui::Control *ctrl);
		int sendOscPort;
		int receiveOscPort;
		string sendOscHost;
		
		ofxOscReceiver receiver;
		ofxOscSender sender;

	};
}