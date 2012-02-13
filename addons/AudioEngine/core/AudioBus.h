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
 *  AudioBus.h, created by Marek Bereza on 09/02/2012.
 */

#pragma once

#include "AudioEffect.h"
#include "StereoSample.h"
#include <vector>

class AudioBus {
public:
	AudioBus();
	
	// you can do a shallow copy of a buffer
	void setBuffer(float *buff, int length, int numChannels);
	
	// or you can allocate a buffer. This only allocates
	// if the requested buffer is a different size to 
	// what's there at the moment.
	void allocateBuffer(int length, int numChannels);
	
	void setVolume(float left, float right);
	
	void addEffect(AudioEffect *effect);
	void accumulateSamples(StereoSample *sample, int length, int numChannels);
	void clip(bool softSaturation, float gain);
	void mix(AudioBus *input);
protected:
	float *buffer;
	float channelVolumes[2];
	void processEffects();
private:
	std::vector<AudioEffect*> effects;
	int length;
	int numChannels;

};
