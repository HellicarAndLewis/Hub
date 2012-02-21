/*
 * SIMPLE PLACE TO PUT ALL THE COLOURS
 *
 * TO ADD A COLOUR:
 *
 * ADD AN ENTRY TO THE ENUM, THEN get/set WITH THE STATIC METHODS!!!
 */

#ifndef WailWall_Colours_h
#define WailWall_Colours_h

#include "ofMain.h"


enum ColourId {

	SURFACE_BG,
	SEARCH_BG,
	HALO_SEARCH,
	HALO_SURFACE,
	NUM_COLORS
};


class Colours {
public:
	
	// this is a singleton
	static int &get(ColourId colour);
	static void set(ColourId, int hexColour);
private:
	Colours();
	static Colours *instance;
	vector<int> colours;
};


#endif
