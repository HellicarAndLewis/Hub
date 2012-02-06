/**
 *  AudioAnalysis.cpp
 *
 *  Created by Marek Bereza on 06/02/2012.
 */


#include "analysis.h"

float tricks::audio::analysis::getMaxAveragePower(WavFile *wav, int windowSize) {
	if(wav->getData()==NULL) {
		printf("audio analysis: wavfile's data is null!\n");
		return 0;
	}
	
	float *data = wav->getData();
	int numSamples = wav->getLength()*wav->getNumChannels();
	float avg = 0;
	
	for(int i = 0; i < numSamples; i += windowSize) {
		float total = 0;
		int ns = MIN(numSamples - i, windowSize);
		for(int j = i; j < i + ns; j++) {
			total += data[j];
		}
		
		float average = total/ns;
		avg = avg>average?avg:average;
	}
	return avg;
}