#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"
#include "ofxCocoaWindow.h"

//========================================================================
int main( ){

	bool bUseCocoaWindow = false;
    
	
	//
	//
	//
	// DO NOT CHANGE THE WINDOW DISPLAY MODE FROM OF_WINDOW!!!
	//
	//
	//
	
    if( bUseCocoaWindow )
    {
        ofxCocoaWindow cocoaWindow;
        ofSetupOpenGL( &cocoaWindow, 800, 600, OF_WINDOW );
    }
    else {
        ofAppGlutWindow window;
        ofSetupOpenGL( &window, 1280, 900, OF_WINDOW );
    }
	
//    ofAppGlutWindow window;
//	ofSetupOpenGL(&window, 1024,768, OF_FULLSCREEN);			// <-------- setup the GL context
//
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());

}
