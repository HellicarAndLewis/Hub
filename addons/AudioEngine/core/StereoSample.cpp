#include "AudioSystem.h"

#ifdef STEREOSAMPLE_USE_SHARED_BUFFER
float StereoSample_buffer[BUFF_SIZE];
#endif


void StereoSample::lerpPanQuad() {
	volume = volume*(1.f-StereoSampleLerpAmt) + targetVolume*StereoSampleLerpAmt;
	pan = pan*(1.f-StereoSampleLerpAmt) + targetPan*StereoSampleLerpAmt;				
	width = targetWidth*(1.f-StereoSampleLerpAmt) + targetWidth*StereoSampleLerpAmt;	
	channelVolumes[0] = getLevel(0.0f, pan, width)*volume;
	channelVolumes[1] = getLevel(0.3333f, pan, width)*volume;
	channelVolumes[2] = getLevel(0.6667f, pan, width)*volume;
	channelVolumes[3] = getLevel(1.0000f, pan, width)*volume;
}


void StereoSample::init() {
	targetPan = pan = 0.5;
	targetVolume = volume = 1.0;
    fadePos = -1;
    setOutputs(0, 1);
	busRef = 0;
}
StereoSample::StereoSample(): Sample() {
	init();
}
	
StereoSample::StereoSample(StereoSample &other): Sample() {
	data = other.getData();
	numChannels = other.getNumChannels();
	length = other.getLength();
	init();
}
	
	
StereoSample::StereoSample(WavFile *wf): Sample() {
	data = wf->getData();
	numChannels = wf->getNumChannels();
	length = wf->getLength();
	init();
}
	

void StereoSample::setWavFile(WavFile *wf) {
	stop();
	data = wf->getData();
	numChannels = wf->getNumChannels();
	length = wf->getLength();
	init();
}
	
void StereoSample::setMix(float _volume, float _pan, float _width) {
	setVolume(_volume);
	setPan(_pan);
	setWidth(_width);
}
	
/** Goes from 0 to 1 for left to right */
void StereoSample::setWidth(float _width) {
	targetWidth = _width;
	if(targetWidth<0) targetWidth = 0;
	if(targetWidth>1) targetWidth = 1;
}
void StereoSample::setPan(float _pan) {
	targetPan = _pan;
	if(targetPan<0) targetPan = 0;
	if(targetPan>1) targetPan = 1;
}
	
void StereoSample::setVolume(float vol) {
	targetVolume = vol;
}
	
	
#ifndef ABS
#define ABS(x) (((x) < 0) ? -(x) : (x))
#endif

#ifndef MAX
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif

/**
 * Lets you calculate a volume value 
 * for a speaker positioned at "pos" for a sample positioned at "pan"
 * with a desired width. all params go from 0 - 1
 */
float StereoSample::getLevel(float pos, float pan, float width) {
	return 	MAX(1.f - 2.f*width*ABS(pos - pan), 0);
}

float StereoSample::getFadeVolume() {

    float outVal = fadePos / fadeDuration * (fadeToVolume - fadeFromVolume) + fadeFromVolume;
    
    
    if(fadeToVolume < fadeFromVolume){
        if( outVal < fadeToVolume ) outVal = fadeToVolume;
        else if( outVal > fadeFromVolume )outVal = fadeFromVolume;
    } else {
        if( outVal > fadeToVolume )outVal = fadeToVolume;
        else if( outVal < fadeFromVolume )outVal = fadeFromVolume;
    }
    return outVal;
}


void StereoSample::addSamplesStereo(float *samples, int length) {
		
	if(length>BUFF_SIZE) {
		printf("Buffer not big enough!!\n");
		memset(samples, 0, length*2*sizeof(float));
		return;
	}
		
	// get the samples to work on
	getSamples(StereoSample_buffer, length);
	float lerpAmt = 0.05f;
    
    if(fadePos>-1) {
        targetVolume = getFadeVolume();
        fadePos += length;
    }
    

	if(getNumChannels()==1) {
		for(int i = 0; i < length; i++) {
            volume = volume*(1.f-lerpAmt) + targetVolume*lerpAmt;
            
			pan = pan*(1.f-lerpAmt) + targetPan*lerpAmt;				
				
			samples[i*2] += StereoSample_buffer[i]*(1 - pan)*volume;
			samples[i*2 + 1] += StereoSample_buffer[i]*pan*volume;
		}
	} else if(getNumChannels()==2) {
		for(int i = 0; i < length; i++) {
				
			volume = volume*(1.f-lerpAmt) + targetVolume*lerpAmt;
			pan = pan*(1.f-lerpAmt) + targetPan*lerpAmt;
				
			samples[i*2] += StereoSample_buffer[i*2]*(1 - pan)*volume;
			samples[i*2 + 1] += StereoSample_buffer[i*2+1]*pan*volume;
		}
	}
    if(fadePos>=fadeDuration) {
        fadePos = -1;
        if(targetVolume<0.001 && volume<0.001) {
            stop();
        }
    }
		
}
void StereoSample::addSamples(float *samples, int length, int numChannels) {
    
	if(length>BUFF_SIZE) {
		printf("Buffer not big enough!!\n");
		memset(samples, 0, length*numChannels*sizeof(float));
		return;
	}
    
	// get the samples to work on
	getSamples(StereoSample_buffer, length);
	float lerpAmt = 0.05f;
    
    if(fadePos>-1) {
        targetVolume = getFadeVolume();
        fadePos += length;
    }
    
    
    // make sure we don't shoot over the channels
    channel1 %= numChannels;
    if(channel2!=-1) channel2 %= numChannels;
    
	if(getNumChannels()==1) {
		for(int i = 0; i < length; i++) {
            volume = volume*(1.f-lerpAmt) + targetVolume*lerpAmt;
            
			pan = pan*(1.f-lerpAmt) + targetPan*lerpAmt;				
            
			samples[i*numChannels + channel1] += StereoSample_buffer[i]*(1 - pan)*volume;
            if(channel2!=-1) 
                samples[i*numChannels + channel2] += StereoSample_buffer[i]*pan*volume;
		}
	} else if(getNumChannels()==2) {
		for(int i = 0; i < length; i++) {
            
			volume = volume*(1.f-lerpAmt) + targetVolume*lerpAmt;
			pan = pan*(1.f-lerpAmt) + targetPan*lerpAmt;
            
			samples[i*numChannels + channel1] += StereoSample_buffer[i*2]*(1 - pan)*volume;
            
			if(channel2!=-1) 
                samples[i*numChannels + channel2] += StereoSample_buffer[i*2+1]*pan*volume;
		}
	}
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    if(fadePos>=fadeDuration) {
        fadePos = -1;
        if(targetVolume<0.001 && volume<0.001) {
            stop();
        }
    }
}


// fades a sample in over a period of [duration] samples
// (also starts it playing)
void StereoSample::fadeIn(float toVolume, int duration) {
	trigger(0);
    fadeToVolume = toVolume;
    fadeFromVolume = 0;
    targetVolume = 0;
    volume = 0;
    fadeDuration = duration;
    fadePos = 0;
}

// fades out and stops the sample
void StereoSample::fadeOut(int duration) {
	//stop();
    fadeToVolume = 0;
    fadeFromVolume = targetVolume;
    fadeDuration = duration;
    fadePos = 0;
}

void StereoSample::fadeTo(float toVolume, int duration) {
	fadeToVolume = toVolume;
    fadeFromVolume = targetVolume;
    fadeDuration = duration;
    fadePos = 0;
}
	

void StereoSample::setOutputs(int channel1, int channel2) {
    this->channel1 = channel1;
    this->channel2 = channel2;
}