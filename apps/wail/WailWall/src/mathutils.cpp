//
//  mathutils.cpp
//  WailWall
//
//  Created by Joel Lewis on 22/02/2012.
//  Copyright (c) 2012 Hellicar & Lewis. All rights reserved.
//

#include "mathutils.h"
#include "ofMain.h"

float smootherStep(float edge0, float edge1, float x)
{
    // Scale, and clamp x to 0..1 range
    x = ofClamp((x - edge0)/(edge1 - edge0), 0.f, 1.f);
    // Evaluate polynomial
    return x*x*x*(x*(x*6 - 15) + 10);
}


float smootherStep(float x)
{
    // Evaluate polynomial
    return x*x*x*(x*(x*6 - 15) + 10);
}