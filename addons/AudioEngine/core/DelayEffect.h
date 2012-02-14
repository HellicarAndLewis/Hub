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
 *  DelayEffect.h, created by Marek Bereza on 13/02/2012.
 */

#pragma once

#include "AudioEffect.h"



class DelayEffect: public AudioEffect {
public:
	static const int MAXDELAYTIME = 22050;
	enum Parameter {
		DELAY_TIME_L,
		DELAY_TIME_R,
		FEEDBACK_L,
		FEEDBACK_R,
		MIX_L,
		MIX_R
	};
	
	DelayEffect() {
		memset(delayL, 0, MAXDELAYTIME*sizeof(float));
		memset(delayR, 0, MAXDELAYTIME*sizeof(float));
		posL = 1;
		posR = 1;
		delayTimeL = MAXDELAYTIME/4;
		delayTimeR = MAXDELAYTIME/4.05;
		feedbackL = 0.7;
		feedbackR = 0.7;
		mixL = 0.5;
		mixR = 0.5;
	}
	
	int getMaxDelayTime() {
		return MAXDELAYTIME;
	}
	
	void setDelayTimes(int l, int r) {
		if(l>MAXDELAYTIME) l = MAXDELAYTIME;
		if(r>MAXDELAYTIME) r = MAXDELAYTIME;
		this->delayTimeL = l;
		this->delayTimeR = r;
	}
	
	void setFeedbacks(float feedbackL, float feedbackR) {
		this->feedbackL = feedbackL;
		this->feedbackR = feedbackR;
	}
	
	void process(float *audio, int length) {
		
		float outL, outR;
		for(int i = 0; i < length; i++) {
			
			posL++;
			posR++;
			posL %= delayTimeL;
			posR %= delayTimeR;
			
			outL = delayL[posL];
			outR = delayR[posR];
			
			delayL[posL] = feedbackL*delayL[posL]+audio[i*2];
			delayR[posR] = feedbackR*delayR[posR]+audio[i*2+1];
			
			audio[i*2  ] += (outL - audio[i*2])*mixL;
			audio[i*2+1] += (outR - audio[i*2+1])*mixR;			
		}
	}
	
	int delayTimeL;
	int delayTimeR;
	float feedbackL;
	float feedbackR;
	float mixL;
	float mixR;
	
	int posL;
	int posR;
	float delayL[MAXDELAYTIME];
	float delayR[MAXDELAYTIME];
	
	float clamp(float in, float min, float max) {
		if(in<min) return min;
		else if (in>max) return max;
		else return in;
	}
	
	void setParameter(int parameterId, float value) {
		switch(parameterId) {
			case DELAY_TIME_L:
				delayTimeL = clamp(value, 0, MAXDELAYTIME);
				break;
			case DELAY_TIME_R:
				delayTimeR = clamp(value, 0, MAXDELAYTIME);
				break;

			case FEEDBACK_L:
				feedbackL = clamp(value, 0, 0.999);
				break;
			
			case FEEDBACK_R:
				feedbackR = clamp(value, 0, 0.999);
				break;
			case MIX_L:
				mixL = clamp(value, 0, 1);
				break;
			case MIX_R:
				mixR = clamp(value, 0, 1);
				break;
		}
	}
	
	void process(float *samples, int length, int numChannels) {
		process(samples, length);
	}
};