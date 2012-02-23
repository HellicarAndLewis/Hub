//
//  mathutils.h
//  WailWall
//
//  Created by Joel Lewis on 22/02/2012.
//  Copyright (c) 2012 Hellicar & Lewis. All rights reserved.
//

#ifndef WailWall_mathutils_h
#define WailWall_mathutils_h
#include "ofMain.h"
float smootherStep(float x);
float smootherStep(float edge0, float edge1, float x); //from http://en.wikipedia.org/wiki/Smoothstep

float rectangleOverlap(const ofRectangle &a, const ofRectangle &b);
#endif
