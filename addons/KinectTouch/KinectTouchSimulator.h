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
 *  KinectTouchSimulator.h, created by Marek Bereza on 01/02/2012.
 */


#include "KinectTouchListener.h"

class KinectTouchSimulator {
public:	
	KinectTouchSimulator();
	void setListener(KinectTouchListener *listener);
	void mousePressed(int x, int y);
	void mouseDragged(int x, int y);
	void mouseReleased(int x, int y);
	void setEnabled(bool enabled);
	bool getEnabled();
	void simulateTouch(float normX, float normY, float depth = 0.75) ;
	void reset();
private:
	float getTouchZ(float time);
	float touchDownTime;
	int idCounter;
	bool enabled;
	KinectTouchListener *listener;
	KinectTouch touch;
	bool mouseIsDown;
};