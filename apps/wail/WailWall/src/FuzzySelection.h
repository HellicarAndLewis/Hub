//
//  FuzzySelection.h
//  WailWall
//
//  Created by Joel Lewis on 21/02/2012.
//  Copyright (c) 2012 Hellicar & Lewis. All rights reserved.
//

#ifndef WailWall_FuzzySelection_h
#define WailWall_FuzzySelection_h

#include "ofMain.h"

class FuzzySelection {
public:
	
	FuzzySelection() {
		value = 0;
		setDuration(1);
		setTarget(0);
		
	}
	
	void setDuration(float duration) {
		this->duration = duration;
		increment = duration/30.f;
	}
	
	void update() {
		//printf("target: %f value: %f\n", target, value);
		if(target==value) return;
		
		if(target>value) {
			value += increment;
			if(target<value) value = target;
		} else {
			value -= increment;
			if(target>value) value = target;
		}
		// clamp
		if(value>1) value = 1;
		else if(value<0) value = 0;
	}
	
	
	void setTarget(float value) {
		target = value;
	}
	
	float getValue() {
		return value;
	}
	
	void setValue(float v) {
		value = v;
	}
	
	void warmUp() {
		value += increment * 2;
		if(value>1) value = 1;
	}
	
private:
	// this how long it would take to get from 0 to 1
	float duration;
	
		   
	// how much we increment every frame	   
	float increment;
		   
   // where we're heading
   float target;

   // where we are
   float value;
	
	
};


#endif
