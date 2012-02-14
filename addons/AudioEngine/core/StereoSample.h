/*
 *  StereoSample.h
 *  AudioTest
 *
 *  Created by Marek Bereza on 01/09/2010.
 *
 */
#pragma once

#include "Sample.h"




#define BUFF_SIZE 16384
#define StereoSampleLerpAmt 0.05f

// you can either use a shared buffer between all sample instances
// or one for each sample. - a shared buffer is ok if you're running
// all instances on a single thread (and much faster if playing lots 
// of sounds
#define STEREOSAMPLE_USE_SHARED_BUFFER


class StereoSample: public Sample {
public:

#ifndef STEREOSAMPLE_USE_SHARED_BUFFER
	float StereoSample_buffer[BUFF_SIZE];
#endif
    
    // index of which channel to play the sound out of
	int channel1;
    
    // optional index for second channel
	int channel2;
	
	int busRef;
	
	// fades a sample in over a period of [duration] samples
	// (also starts it playing)
	void fadeIn(float toVolume, int duration);

	// fades out and stops the sample
	void fadeOut(int duration);
	
	void fadeTo(float toVolume, int duration);
	

	StereoSample();
	StereoSample(StereoSample &other);
	StereoSample(WavFile *wf);
	
	void setWavFile(WavFile *wf);

	void setMix(float _volume, float _pan, float _width);
	
	/** Goes from 0 to 1 for left to right */
	void setWidth(float _width);
	void setPan(float _pan);
	
	void setVolume(float vol);
	
	
	/**
	 * Lets you calculate a volume value 
	 * for a speaker positioned at "pos" for a sample positioned at "pan"
	 * with a desired width. all params go from 0 - 1
	 */
	float getLevel(float pos, float pan, float width);

	void addSamplesStereo(float *samples, int length);

    void addSamples(float *samples, int length, int numChannels);
    
	void setOutputs(int channel1, int channel2 = -1);
	
private:
	void lerpPanQuad();
    float getFadeVolume();
	void init();
	float volume;
	float pan;
	float width;
	float channelVolumes[4];

	float targetVolume;
	float targetPan;
	float targetWidth;
    
    float fadeToVolume;
    float fadeFromVolume;
    float fadeDuration;
    float fadePos;
};