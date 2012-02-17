/* This version of ofxFTGL supports multi byte characters 
 * Rick Companje, 19-12-2011
 */

#pragma once
#include "ofMain.h"
#include <FTGL/ftgl.h>

class ofxFTGLFont {
public:
    ofxFTGLFont();
	
    bool 		loadFont(string filename, float fontsize = 10, bool _bAntiAliased = false, bool _bFullCharacterSet = false, bool makeContours = false);
    void 		setSize(int size);
	const int 	getSize() const;
    void 		setLineHeight(float height);
    ofRectangle getStringBoundingBox(string s, float x, float y);
	bool		isLoaded() const;

    //functions below take in account multi byte characters
    void 		drawString(string s, float x, float y);
    string      getChar(string s, int pos);
    int         getLength(string s);
    int         getWidth(string s);
    
    FTFont*     font;
};

inline bool ofxFTGLFont::isLoaded() const {
	return font != NULL;
}

