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
 *  Slosh.h, created by Marek Bereza on 30/01/2012.
 */

#pragma once

#include "ofMain.h"
#include "AudioSystem.h"

class Slosh {
public:
	float minVol;
	float maxVol;
	float probability;
	Slosh() {
		minVol = 0.25;
		maxVol = 0.5;
		probability = 1;
	}
	
	void setProbability(float probability) {
		this->probability = probability;
	}
	
	void setVolumeRange(float minVol, float maxVol) {
		this->minVol = minVol;
		this->maxVol = maxVol;
	}
	void setup(string path, int number) {
		for(int i = 0; i < number; i++) {
			string file = string(path) + ofToString(i) + ".wav";
			samples.push_back(audio::loadSample(ofToDataPath(file)));
		}
	}
	
	void trigger(ofVec3f p) {

		if(ofRandomuf()<probability) {
			int r = ofRandom(0, samples.size());
			audio::PlayerRef ref = audio::createPlayer(samples[r]);
			audio::setPan(ref, p.x);
			float vol = ofRandom(minVol, maxVol);
			audio::setVolume(ref, vol);
			audio::play(ref);
		}
	}
	vector<audio::SampleRef> samples;
};
