//
//  SineFader.h
//  audioOutputExample
//
//  Created by Marek Bereza on 02/08/2011.
//

#pragma once
#include "AbstractFader.h"
#include "ofMain.h"

class SineFader: public AbstractFader {
public: 
    SineFader();    
    
    float getValue();    
    void reset();

    float modulation;
    float volume;
	float frequency;
    float phase;
private:
    
    float lastTime;
    
};
