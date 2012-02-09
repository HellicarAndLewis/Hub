/**
 *  Slosh.cpp
 *
 *  Created by Marek Bereza on 30/01/2012.
 */

#include "Slosh.h"


Slosh::Slosh() {
	minVol = 0.25;
	maxVol = 0.5;
	probability = 1;
}

void Slosh::setProbability(float probability) {
	this->probability = probability;
}

void Slosh::setVolumeRange(float minVol, float maxVol) {
	this->minVol = minVol;
	this->maxVol = maxVol;
}

bool SloshSortComparator(const Slosh::SloshSample &a, const Slosh::SloshSample &b) {
	return a.volume<b.volume;
}

void Slosh::setup(string path, int number) {
	for(int i = 0; i < number; i++) {
		string file = string(path) + ofToString(i) + ".wav";
		samples.push_back(SloshSample());
		samples.back().sample = audio::loadSample(ofToDataPath(file));
		samples.back().volume = tricks::audio::analysis::getMaxAveragePower(
			audio::getWavFile(
				samples.back().sample
			)
		);
	}
	
	// sort the samples in order of loudness
	sort (samples.begin(), samples.end(), SloshSortComparator);
	/*for(int i = 0; i < samples.size(); i++) {
		printf("[%d] = %f\n", i, samples[i].volume);
	}*/
}

void Slosh::trigger(ofVec3f p) {
	
	if(ofRandomuf()<probability) {

		if(p.z==1) { // if the volume is saturated, play a random sample that is loud
			p.z = ofRandom(0.9, 1.0);
		}
		int r = p.z*(samples.size()-1);
		audio::PlayerRef ref = audio::createPlayer(samples[r].sample);
		audio::setPan(ref, p.x);
		float vol = ofRandom(minVol, maxVol);
		audio::setVolume(ref, vol);
		audio::play(ref);
	}
}
