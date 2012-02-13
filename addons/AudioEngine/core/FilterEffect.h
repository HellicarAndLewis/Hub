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
 *  SVFEffect.h, created by Marek Bereza on 13/02/2012.
 */

#pragma once

#include "AudioEffect.h"
#include "ofMain.h"
#include <math.h>

class FilterEffect: public AudioEffect {
public:
	enum FilterType {
		HI_PASS,
		BAND_PASS,
		NOTCH,
		LOW_PASS
	};
	
	
	
	enum Parameter {
		CUTOFF, // in Hz
		RESONANCE, // [0-1]
		
	};
	void setParameter(int parameterId, float value) {
		if(parameterId==CUTOFF) {
			cutoff = value;
			f = 2 * sin(M_PI*cutoff/fs);
		} else if(parameterId==RESONANCE) {
			q = value;
			if(q<0) q = 0;
			else if(q>0.9) q = 0.9;
		}
	}
	FilterEffect(FilterType type) {
		this->type = type;
		fs = 44100;
		q = 1;
		low[0] = low[1] = high[0] = high[1] = band[0] = band[1] = notch[0] = notch[1] = 0;
	}
	
	void process(float *samples, int length, int numChannels) {
		for(int i = 0; i < length*numChannels; i++) {
			int chan = i%numChannels;
			//samples[i] = ofRandom(-0.05, 0.05);
			low[chan] = low[chan] + f * band[chan];
			high[chan] = q * samples[i] - low[chan] - q * band[chan];
			band[chan] = f * high[chan] + band[chan];
			notch[chan] = high[chan] + low[chan];
			if(type==HI_PASS) {
				samples[i] = high[chan];
			} else if(type==LOW_PASS) {
				samples[i] = low[chan];
			} else if(type==BAND_PASS) {
				samples[i] = band[chan];
			} else {
				samples[i] = notch[chan];
			}
		}
	}
	
	
private:
	float fs;
	FilterType type;
	float cutoff;
	float q;
	float f;
	
	// coeffs
	float low[2];
	float band[2];
	float high[2];
	float notch[2];
};