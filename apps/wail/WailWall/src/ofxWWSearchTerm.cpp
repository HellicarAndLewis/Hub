/*
 *  ofxWWSearchTerm.cpp
 *  WailWall
 *
 *  Created by James George on 1/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxWWSearchTerm.h"
#include "ofxWWTweetParticleManager.h"

ofxWWSearchTerm::ofxWWSearchTerm(){
	tweetsGenerated = false;
	manager = NULL;
}

void ofxWWSearchTerm::draw(){
	manager->sharedLargeFont.drawString(term, pos.x, pos.y);
}