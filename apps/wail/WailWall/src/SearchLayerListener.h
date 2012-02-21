#ifndef SEARCH_LAYER_LISTENERH
#define SEARCH_LAYER_LISTENERH

#include "ofMain.h"

struct SearchTermSelectionInfo {
	string 	term;
	ofVec2f position;
};

class SearchLayerListener {

public:

	virtual void onSearchTermSelected(const SearchTermSelectionInfo& term) = 0;
	
	virtual void onAllSearchTermsDeselected() = 0;
};

#endif