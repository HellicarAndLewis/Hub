//
//  Colours.cpp
//  WailWall
//
//  Created by Joel Lewis on 21/02/2012.
//  Copyright (c) 2012 Hellicar & Lewis. All rights reserved.
//

#include "Colours.h"

Colours *Colours::instance = NULL;

// this is a singleton
int &Colours::get(ColourId colour) {
	if(instance==NULL) {
		instance = new Colours();
	}
	return instance->colours[colour];
}

void Colours::set(ColourId colour, int hexColour) {
	if(instance==NULL) {
		instance = new Colours();
	}
	instance->colours[colour] = hexColour;
}

Colours::Colours() {
	for(int i = 0; i < NUM_COLORS; i++) {
		colours.push_back(0);
	}
}




