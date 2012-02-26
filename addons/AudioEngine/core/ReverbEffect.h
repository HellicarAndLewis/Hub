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
 *  ReverbEffect.h, created by Marek Bereza on 25/02/2012.
 */

#pragma once

#include "AudioEffect.h"
#include "revmodel.hpp"

class ReverbEffect: public AudioEffect {
public:
	
	
		
	
	enum Parameter {
		ROOMSIZE,
		DAMP,
		WET,
		DRY,
		WIDTH,
		MODE
		
	};
	void setParameter(int parameterId, float value) {
		printf("Setting parameter to %f\n", value);
		if(parameterId==ROOMSIZE) {
			model.setroomsize(value);
		}
		else if(parameterId==DAMP) {
			
			model.setdamp(value);
		} else if(parameterId==WET) {
			model.setwet(value);
		} else if(parameterId==DRY) {
			model.setdry(value);
		} else if(parameterId==WIDTH) {
			model.setwidth(value);
		} else if(parameterId==MODE) {
			model.setmode(value);
		}
	}
	ReverbEffect() {
		
	}
	
	void process(float *samples, int length, int numChannels) {
		if(numChannels==2) {
			
			// deinterleave
			for(int i = 0; i < length; i++) {
				bufferL[i] = samples[i*2];
				bufferR[i] = samples[i*2+1];
			}
			
			model.processreplace(bufferL, bufferR, outL, outR, length, 1);
			// interleave
			for(int i = 0; i < length; i++) {
				samples[i*2] = outL[i];
				samples[i*2+1] = outR[i];
			}
		} else {
			// do nothing for now
		}
	}
	float bufferL[8192];
	float bufferR[8192];
	float outL[8192];
	float outR[8192];
	
private:
	revmodel model;
	
};