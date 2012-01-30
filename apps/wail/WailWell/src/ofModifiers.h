/*
 *  ofModifiers.h
 *  WailWall
 *
 *  Created by James George on 1/29/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#ifdef TARGET_WIN32
#define GLUT_BUILDING_LIB
#include "glut.h"
#endif

#ifdef TARGET_OSX
#include "../../../libs/glut/lib/osx/GLUT.framework/Versions/A/Headers/glut.h"
#include <Carbon/Carbon.h>
#endif

#ifdef TARGET_LINUX
#include <GL/glut.h>
#endif

//----------------------------------------
static bool ofGetModifierKeyAlt(){
	return (glutGetModifiers() & GLUT_ACTIVE_ALT);
}

//----------------------------------------
static bool ofGetModifierKeyShift(){	
	return (glutGetModifiers() & GLUT_ACTIVE_SHIFT);
}

//----------------------------------------
static bool ofGetModifierKeyControl(){
	return (glutGetModifiers() & GLUT_ACTIVE_CTRL);	
}

