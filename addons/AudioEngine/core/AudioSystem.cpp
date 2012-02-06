/*
 *  AudioSystem.cpp
 *  AudioTest
 *
 *  Created by Marek Bereza on 01/09/2010.
 *  Copyright 2010 Marek Bereza. All rights reserved.
 *
 */


#include "AudioSystem.h"
#ifdef WIN32
#include <Windows.h>
#include <process.h>
CRITICAL_SECTION  critSec;  	//same as a mutex
#else
#include <pthread.h>
#include <semaphore.h>
pthread_mutex_t  myMutex;
#endif





using namespace audio;

AudioSystem audioSystem;

AudioSystem *audio::getAudioSystem() {
	return &audioSystem;
}

#ifdef USING_RTAUDIO

// audio callback
int audioCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData ) {
			   
  float *buffer = (float *) outputBuffer;
  AudioSystem *as = (AudioSystem *) userData;

  as->getSamples(buffer, nBufferFrames, 2);

  return 0;
}
#endif

AudioSystem::AudioSystem() {
	sampleRefCounter = 0;
	playerRefCounter = 0;
	masterVolume = 1.0;
    softSaturation = false;
    saturationSoftness = 0.5;
#ifdef USING_RTAUDIO
	rtAudio = NULL;
#endif
	enabled = true;
#ifdef WIN32 
	InitializeCriticalSection(&critSec); 
#else 
	pthread_mutex_init(&myMutex, NULL); 
#endif 
	
}


AudioSystem::~AudioSystem() {

#ifndef WIN32 
	pthread_mutex_destroy(&myMutex); 
#endif 
}

void audio::setMasterVolume(float volume, bool softSaturation, float saturationSoftness) {
	if(volume>1.0) volume = 1.0;
	if(volume<0) volume = 0;
	audioSystem.lock();
	audioSystem.masterVolume = volume;
    audioSystem.softSaturation = softSaturation;
    audioSystem.saturationSoftness = saturationSoftness;
	audioSystem.unlock();
}

void audio::init(int numChannels) {
    
    //initialize all the channel volumes
    audioSystem.lock();
    audioSystem.numChannels = numChannels;
    if(audioSystem.channelVolumes!=NULL) delete [] audioSystem.channelVolumes;
    audioSystem.channelVolumes = new float[numChannels];
    for(int i = 0; i < numChannels; i++) {
        audioSystem.channelVolumes[i] = 1.f;
    }
    audioSystem.unlock();
}

#ifdef USING_RTAUDIO
bool audio::begin(int deviceId) {

	audioSystem.lock();
	if(!audioSystem.enabled) return false;
	audioSystem.unlock();
		
	if(audioSystem.rtAudio!=NULL) delete audioSystem.rtAudio;

	audioSystem.rtAudio = new RtAudio();


	unsigned int devices = audioSystem.rtAudio->getDeviceCount();
	cout << "found " << devices << " audio devices"<<endl;
	
	if(deviceId==-1) deviceId = audioSystem.rtAudio->getDefaultOutputDevice();
	
	string currDeviceName = "";
	// Scan through devices for various capabilities
	RtAudio::DeviceInfo info;
	for ( unsigned int i=0; i<devices; i++ ) {
		
		info = audioSystem.rtAudio->getDeviceInfo( i );
		if(deviceId==i) {
			currDeviceName = info.name;
		}
		if ( info.probed == true ) {
			// Print, for example, the maximum number of output channels for each device
			std::cout << "device = " << i << "  - " << info.name;
			std::cout << ": maximum output channels = " << info.outputChannels << "\n";

			
			
    }
  }


	
	cout << "Using device (" << deviceId << ") - " << currDeviceName << endl; 
	RtAudio::StreamParameters parameters;
	parameters.deviceId = deviceId;
	parameters.nChannels = 2;
	parameters.firstChannel = 0;
	unsigned int sampleRate = 44100;
	unsigned int bufferFrames = 1024; // 256 sample frames
		

	try {
		audioSystem.rtAudio->openStream( &parameters, NULL, RTAUDIO_FLOAT32,
                   sampleRate, &bufferFrames, &audioCallback, (void *)&audioSystem );
		audioSystem.rtAudio->startStream();
		return true;
	} catch ( RtError& e ) {
	    cout << "Couldn't connect to device " << endl;
	   	e.printMessage();
	    return false;
	}
}

bool audio::end() {
	audioSystem.lock();
	if(!audioSystem.enabled) return false;
	audioSystem.unlock();
	try {

		if ( audioSystem.rtAudio->isStreamOpen() ) {

			
			audioSystem.rtAudio->closeStream();
			//audioSystem.rtAudio->abortStream();
			
			cout << "Closed audio successfully" << endl;
			// kills the stream immediately - we need to do this because the destructor comes immediately after.
			
			reset();
		}
		return true;
	} catch(RtError& e) {
		cout << "Couldn't close device" << endl;
		e.printMessage();
	}
	return false;
}


#endif
void AudioSystem::lock() {
#ifdef WIN32 
	EnterCriticalSection(&critSec); 
#else 
	pthread_mutex_lock(&myMutex); 
#endif 
}

void AudioSystem::unlock() {
#ifdef WIN32 
	LeaveCriticalSection(&critSec); 
#else 
	pthread_mutex_unlock(&myMutex); 
#endif 
}

void audio::reset() {
	audioSystem.commandList.clear();

	map<SampleRef, WavFile*>::iterator a;
	for(a = audioSystem.audioSamples.begin(); a != audioSystem.audioSamples.end(); a++) {
		delete (*a).second;
	}
	audioSystem.audioSamples.clear();

	map<PlayerRef, StereoSample*>::iterator b;
	for(b = audioSystem.players.begin(); b != audioSystem.players.end(); b++) {
		delete (*b).second;
	}
	audioSystem.players.clear();
}

void audio::play(PlayerRef playerId, float timeDelay) {
	if(playerId==0) {
        printf("Trying to paly a zero\n");
    }
	audioSystem.addCommand(playerId, PLAY, timeDelay);

}


WavFile *audio::getWavFile(SampleRef sampleId) {
	return audioSystem.audioSamples[sampleId];
}


/**
 * Stops a sample from playing
 */
void audio::stop(PlayerRef playerId) {
	audioSystem.addCommand(playerId, STOP);
}


/**
 * Sets whether a sample is looping or not.
 */
void audio::setLooping(PlayerRef playerId, bool looping) {
	audioSystem.addCommand(playerId, LOOP, looping ? 1.0f : 0.0f);
}

void audio::setMix(PlayerRef playerId, float volume, float pan) {
	audioSystem.lock();
	audioSystem.commandList.push_back(AudioCommand(playerId, VOLUME, volume));
	audioSystem.commandList.push_back(AudioCommand(playerId, PAN, pan));
	audioSystem.unlock();
}


void audio::setPan(PlayerRef playerId, float pan) {
	audioSystem.addCommand(playerId, PAN, pan);
}

void audio::setVolume(PlayerRef playerId, float volume) {
	audioSystem.addCommand(playerId, VOLUME, volume);
}

void audio::setOutput(PlayerRef playerId, int channel1, int channel2) {
    audioSystem.addCommand(playerId, OUTPUT, channel1, channel2);
}


void AudioSystem::addCommand(PlayerRef playerId, CommandType type, float parameter1, float parameter2) {
	lock();
	commandList.push_back(AudioCommand(playerId, type, parameter1, parameter2));
	unlock();
}

int secondsToSamples(float seconds) {
	float samples = 44100.f*seconds;
	return (int) samples;
}

void AudioSystem::getSamples(float *samples, int length, int numChannels) {

	memset(samples, 0, numChannels*sizeof(float)*length);
	
	lock();
	if(!enabled) return;
	for(unsigned int i = 0; i < commandList.size(); i++) {
		if(commandList[i].playerId==0 || players.find(commandList[i].playerId)==players.end()) {
            printf("Trying to %d an unallocated sample!!\n", commandList[i].type);

			continue;
		}
		
		
		switch(commandList[i].type) {
				
			case PLAY:
				players[commandList[i].playerId]->trigger(-secondsToSamples(commandList[i].parameter1));
				break;
            case OUTPUT:
                players[commandList[i].playerId]->setOutputs(commandList[i].parameter1, commandList[i].parameter2);
                break;
				
			case FADE_IN:
				players[commandList[i].playerId]->fadeIn(commandList[i].parameter1, secondsToSamples(commandList[i].parameter2));
				break;

			case FADE_TO:
				players[commandList[i].playerId]->fadeTo(commandList[i].parameter1, secondsToSamples(commandList[i].parameter2));
				break;

			case FADE_OUT:
				players[commandList[i].playerId]->fadeOut(secondsToSamples(commandList[i].parameter1));
				break;

			case STOP:
				players[commandList[i].playerId]->stop();
				break;
				
			case LOOP:
				players[commandList[i].playerId]->setLooping(commandList[i].parameter1==1);
				break;
				
			case VOLUME:
				players[commandList[i].playerId]->setVolume(commandList[i].parameter1);
				break;
				
			case PAN:
				players[commandList[i].playerId]->setPan(commandList[i].parameter1);
				break;
				
		}
	}
	
	commandList.clear();

	// MW removed unlock to fix crash, correct?
	//unlock();
	 
	map<PlayerRef, StereoSample*>::iterator it;
	vector<PlayerRef> playersToDelete;
	
	//if(numChannels==2) {
    for(it = players.begin(); it != players.end(); ++it) {
        if((*it).second->isPlaying()) {
            (*it).second->addSamples(samples, length, numChannels);
        } else {
            if((*it).second->donePlaying()) { // has it ever played?
                playersToDelete.push_back((*it).first);
            }
        }
    }
    
    // delete any players that have finished playing
    for(int i = 0; i < playersToDelete.size(); i++) {
        delete players[playersToDelete[i]];
        players.erase(playersToDelete[i]);
    }
		
	//}

	int numSamples = length * numChannels;

	// MW removed unlock to fix crash, correct?
	//lock();

    float saturationCoefficient = 1 + 9*saturationCoefficient;
    if(saturationCoefficient>10) saturationCoefficient = 10;
    else if(saturationCoefficient<1) saturationCoefficient = 1;

    float satCoeffMinusOne = (saturationCoefficient-1);
    
	for(int i = 0; i <numSamples; i++) {
        samples[i] *= masterVolume * channelVolumes[i%numChannels];
        
        // do soft sat here maybe.
        if(softSaturation) {
            float absX = samples[i]>0?samples[i]:-samples[i];
            samples[i] = samples[i]*(absX + saturationCoefficient)/(samples[i]*samples[i] + satCoeffMinusOne*absX + 1);
        }
        
        // hard clipping, and clip monitoring
        if(samples[i]>1) {
            clipped = true;
            samples[i] = 1;
        } else if(samples[i]<-1) {
            clipped = true;
            samples[i] = -1;
        }
    }
    
	unlock();
}

void audio::setEnabled(bool enabled) {
	
    
	bool _enabled = enabled;

	audioSystem.lock();

	audioSystem.enabled = enabled;
#ifdef USING_RTAUDIO
	if(!_enabled && audioSystem.enabled) end();
	else if(_enabled && !audioSystem.enabled) begin();
#endif
	audioSystem.unlock();
}


/**
 * Loads a sample into RAM, and returns a reference that 
 * can be used for creating sample players
 */
SampleRef audio::loadSample(string file) {

    // don't load if we've already loaded
    if(audioSystem.loadedSamples.find(file)!=audioSystem.loadedSamples.end()) {
        return audioSystem.loadedSamples[file];
    }
    
    
	WavFile *wf = new WavFile();
	if(wf->load(file)) {
		audioSystem.sampleRefCounter++;
		audioSystem.audioSamples[audioSystem.sampleRefCounter] = wf;
        audioSystem.loadedSamples[file] = audioSystem.sampleRefCounter;
		return audioSystem.sampleRefCounter;
	} else {
		cout << "Failed to load audio file: "<< file << endl;
		return 0;
	}
}

bool audio::hasClipped() {
    audioSystem.lock();
    bool clipped = audioSystem.hasClipped();
    audioSystem.unlock();
    return clipped;
}



bool audio::isPlaying(PlayerRef playerId) {
	bool pl = false;
	audioSystem.lock();
	if(audioSystem.players.find(playerId)!=audioSystem.players.end()) {
		pl = audioSystem.players[playerId]->isPlaying();
	}
	audioSystem.unlock();
	return pl;
}


void audio::setChannelVolume(int channel, float volume) {
    audioSystem.lock();
    audioSystem.channelVolumes[channel] = volume;
    audioSystem.unlock();
}

/**
 * Creates a sample player and returns a reference
 * to be used for playing back samples.
 */
PlayerRef audio::createPlayer(SampleRef sampleId) {
	// MW required?
	audioSystem.lock();

	if(sampleId==0) {
		cout << "AudioSystem::createPlayer(): Trying to load invalid sample! It probably means the file was not found or is corrupt." << endl;
		return 0;
	}
	
	if(audioSystem.audioSamples.find(sampleId)==audioSystem.audioSamples.end()) {
		cout << "AudioSystem::createPlayer(): The sample was not loaded." << endl;
		return 0;
	}
	
	audioSystem.playerRefCounter++;
	StereoSample *sample = new StereoSample(audioSystem.audioSamples[sampleId]);
	audioSystem.players[audioSystem.playerRefCounter] = sample;

	// MW required?
	audioSystem.unlock();

	return audioSystem.playerRefCounter;
}




/**
 * Fades in and plays a sample. Kind of like a soft version of play()
 */
void audio::fadeIn(PlayerRef playerId, float toVolume, float duration) {
	audioSystem.addCommand(playerId, FADE_IN, toVolume, duration);
}
	


/**
 * Sets which channel we should play the audio out of.
 */
void audio::setChannel(PlayerRef playerId, int channel1, int channel2) {
	audioSystem.lock();
	audioSystem.players[playerId]->channel1 = channel1;

	if(channel2!=-1) { // this one's stereo
		audioSystem.players[playerId]->channel2 = channel2;
	}
	
	audioSystem.unlock();
}
		

/**
 * this lets you fade a sound to a certain volume. It doesn't
 * stop playing the sample, just fades the volume.
 */
void audio::fadeTo(PlayerRef playerId, float toVolume, float duration) {
	audioSystem.addCommand(playerId, FADE_TO, toVolume, duration);
}
	

/**
 * This fades out (and stops) a sample.
 */
void audio::fadeOut(PlayerRef playerId, float duration) {
	audioSystem.addCommand(playerId, FADE_OUT, duration);
}
	
	
bool AudioSystem::hasClipped() {
    if(clipped) {
        clipped = false;
        return true;
    } else {
        return false;
    }
}