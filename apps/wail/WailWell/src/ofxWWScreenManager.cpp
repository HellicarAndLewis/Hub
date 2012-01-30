/*
 *  ofxWWScreenManager.cpp
 *  WailWell
 *
 *  Created by James George on 1/29/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofxWWScreenManager.h"
#include "ofxXmlSettings.h"

ofxWWScreenManager::ofxWWScreenManager(){
	inited = false;
	editingMode = false;
}

void ofxWWScreenManager::enableEditing(){
	
}

void ofxWWScreenManager::disableEditing(){

}

void ofxWWScreenManager::generateScreens(int screensWide, int screensTall, ofOrientation orientation){
	
	//TODO add system warning dialogs or something because this will reset everything
	screens.clear();

	float baseSourceScreenWidth = sourceRect.width/screensWide;
	float baseSourceScreenHeight = sourceRect.height/screensTall;
	float baseDestScreenWidth = destRect.width/screensWide;
	float baseDestScreenHeight = destRect.height/screensTall;
	for(int x = 0; x < screensWide; x++){
		for(int y = 0; y < screensTall; y++){
			ofxWWScreen screen;
			screen.sourceRect = ofRectangle(x*baseSourceScreenWidth,
											y*baseSourceScreenHeight,
											baseSourceScreenWidth, baseSourceScreenHeight);
			
			if(orientation == OF_ORIENTATION_DEFAULT){
				screen.destRect = ofRectangle(destRect.x + x*baseDestScreenWidth,
											  destRect.y + y*baseDestScreenHeight,
											  baseDestScreenWidth, baseDestScreenWidth);			
				
			}
			//Doing it this way first, we are assuming the output is rotated
			else if(orientation == OF_ORIENTATION_90_LEFT) {				
				//TODO:
			}
			else if(orientation == OF_ORIENTATION_90_RIGHT) {				
				screen.destRect = ofRectangle(y*(destRect.y + y/destRect.height),
											  x*(destRect.x + x/destRect.width),
											  destRect.height/screensTall,
											  destRect.width/screensWide);			
			}				
			
			screen.orientation = orientation;
			
			screens.push_back( screen );
		}
	}
	
	inited = true;
}

void ofxWWScreenManager::renderScreens(){
	ofSetColor(255, 255, 255);
	vector<ofVec2f> positions;
	vector<ofVec2f> textureCoords;
	for(int i = 0; i < screens.size(); i++){
		//add all the vertices from the rectangle
		positions.push_back( ofVec2f(screens[i].destRect.x,screens[i].destRect.y) );
		positions.push_back( ofVec2f(screens[i].destRect.x+screens[i].destRect.width,screens[i].destRect.y) );
		positions.push_back( ofVec2f(screens[i].destRect.x+screens[i].destRect.width,screens[i].destRect.y+screens[i].destRect.height) );
		positions.push_back( ofVec2f(screens[i].destRect.x,screens[i].destRect.y+screens[i].destRect.height) );
		
		//the texture coordinates depend on orientation
		if(screens[i].orientation == OF_ORIENTATION_DEFAULT){
			textureCoords.push_back( ofVec2f(screens[i].sourceRect.x,screens[i].sourceRect.y) );
			textureCoords.push_back( ofVec2f(screens[i].sourceRect.x+screens[i].sourceRect.width,screens[i].sourceRect.y) );
			textureCoords.push_back( ofVec2f(screens[i].sourceRect.x+screens[i].sourceRect.width,screens[i].sourceRect.y+screens[i].sourceRect.height) );
			textureCoords.push_back( ofVec2f(screens[i].sourceRect.x,screens[i].sourceRect.y+screens[i].sourceRect.height) );			
		}
		else if(screens[i].orientation == OF_ORIENTATION_90_LEFT){
			textureCoords.push_back( ofVec2f(screens[i].sourceRect.x,screens[i].sourceRect.y+screens[i].sourceRect.height) );			
			textureCoords.push_back( ofVec2f(screens[i].sourceRect.x,screens[i].sourceRect.y) );
			textureCoords.push_back( ofVec2f(screens[i].sourceRect.x+screens[i].sourceRect.width,screens[i].sourceRect.y) );
			textureCoords.push_back( ofVec2f(screens[i].sourceRect.x+screens[i].sourceRect.width,screens[i].sourceRect.y+screens[i].sourceRect.height) );
		}
		else if(screens[i].orientation == OF_ORIENTATION_90_RIGHT){		
			textureCoords.push_back( ofVec2f(screens[i].sourceRect.x+screens[i].sourceRect.width,screens[i].sourceRect.y) );
			textureCoords.push_back( ofVec2f(screens[i].sourceRect.x+screens[i].sourceRect.width,screens[i].sourceRect.y+screens[i].sourceRect.height) );
			textureCoords.push_back( ofVec2f(screens[i].sourceRect.x,screens[i].sourceRect.y+screens[i].sourceRect.height) );			
			textureCoords.push_back( ofVec2f(screens[i].sourceRect.x,screens[i].sourceRect.y) );
		}		
	}
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, &positions[0].x);
	glTexCoordPointer(2, GL_FLOAT, 0, &textureCoords[0].x);
	glDrawArrays(GL_QUADS, 0, screens.size()*4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	

}

void ofxWWScreenManager::renderPreview(){
	
	ofPushStyle();
	ofNoFill();
	
	
	for(int i = 0; i < screens.size(); i++){
		ofSetColor(255, 255, 0, 120);
		ofRectangle previewRect(sourceRect.x + screens[i].sourceRect.x, sourceRect.y + screens[i].sourceRect.y, 
								screens[i].sourceRect.width, screens[i].sourceRect.height);
		ofRect(previewRect);
		string rectString = "r" + ofToString(i) + "\n" + ofToString(screens[i].sourceRect.x, 1) + "," +  ofToString(screens[i].sourceRect.y, 1) + "\n" + ofToString(screens[i].sourceRect.width, 1) + "," + ofToString(screens[i].sourceRect.height, 1) + "";
		if(screens[i].orientation == OF_ORIENTATION_90_RIGHT){
			rectString += "\nR";
		}
		else if(screens[i].orientation == OF_ORIENTATION_90_LEFT){
			rectString += "\nL";
		}
		
		ofSetColor(255, 255, 255);
		ofDrawBitmapString(rectString, previewRect.x+5, previewRect.y+15);
	}
	
	ofPopStyle();
}

void ofxWWScreenManager::displayDestinationGui(){
//	ofPushStyle();
//	ofNoFill();
//	ofSetColor(255, 0, 0);
//	
//	for(int i = 0; i < screens.size(); i++){
//		
//		ofRectangle drawRect;
//		drawRect.x = ofMap(screens[i].destRect.x, destRect.x,destRect.x+destRect.width,
//						   previewRect.x, previewRect.x+previewRect.width);
//		drawRect.y = ofMap(screens[i].destRect.y, fullDestinationRectangle.y,fullDestinationRectangle.y+fullDestinationRectangle.height,
//						   previewRect.y, previewRect.y+previewRect.height);
//		
//		drawRect.width  = screens[i].destRect.width  * previewRect.width  / fullDestinationRectangle.width; 
//		drawRect.height = screens[i].destRect.height * previewRect.height / fullDestinationRectangle.height;
//		
//		ofRect(drawRect);
//		
//	}
//	
//	ofPopStyle();
	
}

void ofxWWScreenManager::loadScreens(string xmlFile){
	ofxXmlSettings settings;
	if(settings.loadFile(xmlFile)){
		screens.clear();
		settings.pushTag("layout");
		settings.pushTag("source");
		sourceRect = ofRectangle(settings.getValue("x", 0), settings.getValue("y", 0), 
								 settings.getValue("width", 0), settings.getValue("height", 0));		
		settings.popTag();//source
		
		settings.pushTag("dest");
		destRect = ofRectangle(settings.getValue("x", 0), settings.getValue("y", 0), 
							   settings.getValue("width", 0), settings.getValue("height", 0));				
		settings.popTag(); //dest
		
		settings.pushTag("screens");
		int numScreens = settings.getNumTags("screen");
		for(int i = 0; i < numScreens; i++){
			ofxWWScreen screen;
			settings.pushTag("screen", i);
			
			settings.pushTag("source");
			screen.sourceRect = ofRectangle(settings.getValue("x", 0), settings.getValue("y", 0), 
									 settings.getValue("width", 0), settings.getValue("height", 0));		
			settings.popTag();//source
			
			settings.pushTag("dest");
			screen.destRect = ofRectangle(settings.getValue("x", 0), settings.getValue("y", 0), 
								   settings.getValue("width", 0), settings.getValue("height", 0));				
			settings.popTag(); //dest
			
			screen.orientation = (ofOrientation)settings.getValue("orientation", OF_ORIENTATION_DEFAULT);
			
			settings.popTag(); //screen
			
			screens.push_back( screen );
			
		}
		settings.popTag();
		inited = true;
	}
}

void ofxWWScreenManager::saveScreens(string xmlFile){
	
	ofxXmlSettings settings;
	settings.addTag("layout");
	settings.pushTag("layout");
	
	settings.addTag("source");
	settings.pushTag("source");
	settings.addValue("x", sourceRect.x);
	settings.addValue("y", sourceRect.y);
	settings.addValue("width", sourceRect.width);
	settings.addValue("height", sourceRect.height);
	settings.popTag(); //source

	settings.addTag("dest");
	settings.pushTag("dest");
	settings.addValue("x", destRect.x);
	settings.addValue("y", destRect.y);
	settings.addValue("width", destRect.width);
	settings.addValue("height", destRect.height);
	settings.popTag(); //dest
	
	settings.addTag("screens");
	settings.pushTag("screens");
	for(int i = 0; i < screens.size(); i++){
		settings.addTag("screen");
		settings.pushTag("screen", i);
		
		settings.addTag("source");
		settings.pushTag("source");
		settings.addValue("x", screens[i].sourceRect.x);
		settings.addValue("y", screens[i].sourceRect.y);
		settings.addValue("width", screens[i].sourceRect.width);
		settings.addValue("height", screens[i].sourceRect.height);
		settings.popTag(); //source

		settings.addTag("dest");
		settings.pushTag("dest");
		settings.addValue("x", screens[i].destRect.x);
		settings.addValue("y", screens[i].destRect.y);
		settings.addValue("width", screens[i].destRect.width);
		settings.addValue("height", screens[i].destRect.height);
		settings.popTag(); //dest
		
		settings.popTag(); //screen
		
		settings.addValue("orientation", screens[i].orientation);
	}
	
	settings.popTag();//screens
	
	settings.popTag(); //layout
	
	settings.saveFile(xmlFile);
}


//void ofxWWScreenManager::loadScreens(){
//	loaded = true;
//}
//
//void ofxWWScreenManager::saveScreens(){
//	//TODO:
//}

//void ofxWWScreenManager::mousePressed(ofMouseEventArgs& args){
//	
//}
//
//void ofxWWScreenManager::mouseMoved(ofMouseEventArgs& args){
//	
//}
//
//void ofxWWScreenManager::mouseDragged(ofMouseEventArgs& args){
//	
//}
//
//void ofxWWScreenManager::mouseReleased(ofMouseEventArgs& args){
//	
//}
//
//void ofxWWScreenManager::keyPressed(ofKeyEventArgs& args){
//	
//}
