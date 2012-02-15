/**
 *  AudioBus.cpp
 *
 *  Created by Marek Bereza on 09/02/2012.
 */

#include "AudioBus.h"
#include "ofMain.h"

AudioBus::AudioBus() {
	this->channelVolumes[0] = 1;
	this->channelVolumes[1] = 1;
	this->buffer = NULL;
	this->length = 0;
	this->numChannels = 0;
}

void AudioBus::setBuffer(float *buffer, int length, int numChannels) {
	//if(this->buffer!=NULL) delete [] this->buffer;
	this->buffer = buffer;
	this->length = length;
	this->numChannels = numChannels;
	
}
void AudioBus::allocateBuffer(int length, int numChannels) {
	if(this->length!=length || this->numChannels!=numChannels) {
		delete [] buffer;
		buffer = NULL;
	}
	
	if(buffer==NULL) {
		buffer = new float[length*numChannels];
	}
	
	this->length = length;
	this->numChannels = numChannels;
	
	memset(buffer, 0, length*numChannels*sizeof(float));
}
void AudioBus::setVolume(float left, float right) {
	this->channelVolumes[0] = left;
	this->channelVolumes[1] = right;
}

void AudioBus::addEffect(AudioEffect *effect) {
	effects.push_back(effect);
}

void AudioBus::accumulateSamples(StereoSample *sample, int length, int numChannels) {
	sample->addSamples(buffer, length, numChannels);
	
}


void AudioBus::clip(bool softSaturation, float gain) {
	int nl = numChannels*length;
	
	
	float saturationCoefficient = 5;//1 + 9*saturationCoefficient;
    if(saturationCoefficient>10) saturationCoefficient = 10;
    else if(saturationCoefficient<1) saturationCoefficient = 1;
	
    float satCoeffMinusOne = (saturationCoefficient-1);
	
	
	for(int i = 0; i <nl; i++) {
        buffer[i] *= gain * channelVolumes[i%numChannels];
        
        // do soft sat here maybe.
        if(softSaturation) {
            float absX = buffer[i]>0?buffer[i]:-buffer[i];
            buffer[i] = buffer[i]*(absX + saturationCoefficient)/(buffer[i]*buffer[i] + satCoeffMinusOne*absX + 1);
        }
        
        // hard clipping, and clip monitoring
        if(buffer[i]>1) {
//            clipped = true;
            buffer[i] = 1;
        } else if(buffer[i]<-1) {
 //           clipped = true;
            buffer[i] = -1;
        }
    }
}

void AudioBus::processEffects() {
	for(int i = 0; i < effects.size(); i++) {
		effects[i]->process(buffer, length, numChannels);
	}
}
void AudioBus::mix(AudioBus *input) {
	// run the effects,
	input->processEffects();
	
	// then mix the samples from the input into this bus
	int nl = numChannels * length;
	float *channelVols = input->channelVolumes;
	for(int i = 0; i < nl; i++) {
		buffer[i] += input->buffer[i] * channelVols[i%numChannels];
	}
}