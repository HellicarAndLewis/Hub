/*
 *  AudioSystem.h
 *  AudioTest
 *
 *  Created by Marek Bereza on 01/09/2010.
 *
 */

#pragma once

#include <string.h>
#include <string>
#include <vector>
#include <map>
#include "WavFile.h"
#include "StereoSample.h"
#include "AudioBus.h"
#include "AudioEffect.h"

#include "SVFEffect.h"
#include "DelayEffect.h"

#ifdef USING_RTAUDIO
#include "RtAudio.h"
#endif
using namespace std;


namespace audio {
	
	
	enum EffectType {
			// delay, params:
			// 0: delay time
			// 1: feedback
			// 2: mix
			EFFECT_TYPE_DELAY,
		
			// filters
			// 0: cutoff
			// 1: resonance
			EFFECT_TYPE_HI_PASS,
			EFFECT_TYPE_LOW_PASS,
			EFFECT_TYPE_BAND_PASS,
			EFFECT_TYPE_NOTCH
		
	};
	
	
	/** this is the type for the id of a sample in RAM */
	typedef unsigned int SampleRef;
	
	/* This references a player that points to a sample in RAM */
	typedef unsigned int PlayerRef;
		
	/* This is a bus that you can play a PlayerRef through */
	typedef unsigned int BusRef;
	
	/* This references an effect that you can put in a bus */
	typedef unsigned int EffectRef;
	
	
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 
	// STARTING/STOPPING
	// 
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** You must tell the audio engine how many channels you are using with this */
    void init(int numChannels);
    
#ifdef USING_RTAUDIO
	/** Starts the audio stream. Returns true on success. */
	bool begin(int audioDeviceId = -1);
		
	/** Stops the audio stream. Returns true on success. */
	bool end();
#endif
    
	/**
	 * Disposes all the samples in the system, and resets everything. Automatically called on end();
	 */
	void reset();
		
	
	
	
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 
	// INSTANTIATION OF SAMPLES AND PLAYERS
	// 
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	/**
	 * Loads a sample into RAM, and returns a reference that 
	 * can be used for creating sample players. Returns 0 if 
	 * the audio file could not be loaded.
	 */
	SampleRef loadSample(string file);
	
	/**
	 * Creates a sample player and returns a reference
	 * to be used for playing back samples.
	 */
	PlayerRef createPlayer(SampleRef sampleId);
	
	
	/**
	 * If you want effects, you create a bus, and add effects to it.
	 * You have to specify a channel for its output
	 */
	BusRef createBus(int channel);
	
	/**
	 * Sets the bus volume. If you don't set the right channel volume
	 * it'll default to whatever volumeL is.
	 */
	void setBusVolume(BusRef busRef, float volumeL, float volumeR = -1);
	
	/**
	 * Creates an effect assigned to a specific bus
	 */
	EffectRef createEffect(BusRef busRef, EffectType effectType);
	
	void setEffectParameter(EffectRef effect, int parameter, float value);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 
	// PLAYING STUFF
	// 
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	/**
	 * Tells you if a sample is still playing or not
	 */
	bool isPlaying(PlayerRef playerId);
	

	/**
	 * Starts playing a sample. timeDelay is in seconds and means 
	 * that the sample will start playing after a delay of timeDelay seconds
	 */ 
	void play(PlayerRef playerId, float timeDelay = 0);
	
	/**
	 * Play a sound through a specific bus
	 */
	void playOnBus(PlayerRef playerId, BusRef busRef);
	
	
	/**
	 * Fades in and plays a sample. Kind of like a soft version of play()
	 */
	void fadeIn(PlayerRef playerId, float toVolume, float duration);
	
	
	/**
	 * Stops a sample from playing. When you stop a player, 
	 * it will delete the player but not the actual sample.
	 */
	void stop(PlayerRef playerId);
	
	
	/**
	 * This fades out (and stops) a sample.
	 */
	void fadeOut(PlayerRef playerId, float duration);
	
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 
	// MIXING - you can call all these functions at any time while the sample is playing.
	// 
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
    /**
     * This sets the volume of a specific output channel, not a specific sound. Volume goes from 0 to 1
     */
    void setChannelVolume(int channel, float volume);
    
	/**
	 * Sets which channel we should play the audio out of.
	 */
	void setChannel(PlayerRef playerId, int channel1, int channel2 = -1);
		
	
		
	/**
	 * Sets whether a sample is looping or not.
	 */
	void setLooping(PlayerRef playerId, bool looping);
	
	
	/**
	 * Sets the volume and pan at the same time
	 */
	void setMix(PlayerRef playerId, float volume, float pan);
	
	/**
	 * Sets the pan for an individual sound if it has 2 outputs
	 */
	void setPan(PlayerRef playerId, float pan);
	
	/**
	 * Sets the volume for an individual sound
	 */
	void setVolume(PlayerRef playerId, float volume);
	
	
	/**
	 * this lets you fade a sound to a certain volume. It doesn't
	 * stop playing the sample, just fades the volume.
	 */
	void fadeTo(PlayerRef playerId, float toVolume, float duration);
	
    
    /**
     * This lets you set the output channel of a sound - optionally, 
     * you can set 2 different channels
     */
	void setOutput(PlayerRef playerId, int channel1, int channel2 = -1);
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 
	// GLOBAL CONTROL
	// 
	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	/**
	 * Turns the sound on and off 
	 */
	void setEnabled(bool enabled);

	/**
	 * Sets the master volume for the whole system
     * You can also set whether to use soft saturation here.
     * Saturation softness goes from 0 to 1
     * 
	 */
	void setMasterVolume(float volume, bool softSaturation = false, float saturationSoftness = 0.5);

	
	
	/**
     * Returns true if there's been sound clipping since the last time hasClipped() was called.
     */
	bool hasClipped();
	
	
	

	
	
	
	
	
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// You don't need to know anything beyond here
	//
    
	WavFile *getWavFile(SampleRef sampleId);
	
    SampleRef getSampleByFileName(string file);
    
	enum CommandType {
		PLAY,
		STOP,
		LOOP,
		VOLUME,
		PAN,
		FADE_IN,
		FADE_OUT,
		FADE_TO,
        OUTPUT,
		PLAY_ON_BUS,
		SET_BUS_VOLUME,
		SET_EFFECT_PARAM
	};
		

	class AudioCommand {
	public:
		AudioCommand(PlayerRef _playerId = 0, CommandType _type = PLAY, float _parameter1 = 0, float _parameter2 = 0) {
			playerId = _playerId;
			type = _type;
			parameter1 = _parameter1;
			parameter2 = _parameter2;
		}

		PlayerRef playerId;
		CommandType type ;

		// optional user value
		float parameter1;
		float parameter2;
	};

	class AudioSystem {
	public:
		AudioSystem();
		~AudioSystem();
		/**
		 * Gets the audio buffer for playback
		 */
		void getSamples(float *samples, int length, int numChannels);

        // this is basically the mixer
		float *channelVolumes;
        int numChannels;

		float masterVolume;
        bool clipped;

        // whether to use soft saturation
        bool softSaturation;

        // how much saturation.
        float saturationSoftness;
        
        bool hasClipped();
		void lock();
		void unlock();
		
		bool enabled;
		
		/**
		 * Locks, adds a command then unlocks
		 */
		void addCommand(PlayerRef playerId, CommandType type, float parameter = 0, float parameter2 = 0);
		
		
		vector<AudioCommand> commandList;
		map<SampleRef, WavFile*> audioSamples;
		map<PlayerRef, StereoSample*> players;

		map<BusRef, AudioBus*> buses;
		map<EffectRef, AudioEffect*> effects;
		
		
        // convenience lookup map to see if we've already loaded an audio file.
		map<string,SampleRef> loadedSamples;
        
		// increment every time a sample is added
		SampleRef sampleRefCounter;
		
		// increment every time a player is added
		PlayerRef playerRefCounter;
		
		// increment every time a bus is added
		BusRef busRefCounter;
		
		EffectRef effectRefCounter;
		
#ifdef USING_RTAUDIO
		// implementation
		RtAudio *rtAudio;
#endif
	};
	
	AudioSystem *getAudioSystem();
}

