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



float GetRectangleOverlap( float minx1, float miny1,
								float maxx1, float maxy1, float minx2, float miny2,
								float maxx2, float maxy2 )
{
	float overlap;
	printf("minx1: %f, miny1: %f, maxx1: %f, maxy1: %f, minx2: %f, miny2: %f, maxx2:%f, maxxy2: %f\n"
			,minx1, miny1, maxx1, maxy1, minx2, miny2, maxx2, maxy2);
	if ( minx1 > maxx2 )
		overlap = 0.0f;
	else if ( maxx1 < minx2 )
		overlap = 0.0f;
	else if ( miny1 > maxy2 )
		overlap = 0.0f;
	else if ( maxy1 < miny2 )
		overlap = 0.0f;
	else
	{
		// calculate overlap area
		int x_overlap = MIN( maxx2, maxx1 )-MAX( minx2, minx1 );
		int y_overlap = MIN( maxy2, maxy1 )-MAX( miny2, miny1 );
		overlap = x_overlap*y_overlap;
	}
	
	float area1 = (maxx1-minx1)*(maxy1-miny1);
	float area2 = (maxx2-minx2)*(maxy2-miny2);
	float area_norm = (area1+area2)/(2.0f*area1);
	
	// convert to a percentage
	// return overlap/area_norm;
	return overlap/area1;
	
}


float rectangleOverlap(const ofRectangle &a, const ofRectangle &b) {
	return GetRectangleOverlap(a.x, a.y, a.x + a.width, a.y + a.height, 
							   b.x, b.y, b.x+b.width, b.y+b.height);
}
