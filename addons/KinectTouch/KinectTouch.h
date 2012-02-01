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
 *  Description: Represents a touch made by a hand in a kinect. It's in 3d, and normalized [0-1]
 *				 
 *  KinectTouch.h, created by Marek Bereza on 31/01/2012.
 */

#pragma once

#include "ofMain.h"

/**
 * inherits from ofVec3f so you can use x, y and z (normalized [0-1])
 */
class KinectTouch: public ofVec3f {
public:	
	// the persistent ID
	int id;
	
	// current velocity - should be 0 on touchDown
	ofVec3f vel;
	
	// the normalized width + normalized height / 2 
	// gives you a rough indication of size of blob.
	// should go from 0-1 - 1 is the whole kinect
	// field of view, so blobs probably won't be much
	// bigger than 0.1
	float size;
};