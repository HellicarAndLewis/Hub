/**
 *  ofxSimpleGuiHexColorPicker.cpp
 *
 *  Created by Marek Bereza on 20/02/2012.
 */

#include "ofxSimpleGuiHexColorPicker.h"


ofxSimpleGuiHexColorPicker::ofxSimpleGuiHexColorPicker(string name, int &hexColor) : ofxSimpleGuiControl(name) {
	this->hexColor = &hexColor;
	
	controlType = "HexColorPicker";
	setup();
}

void ofxSimpleGuiHexColorPicker::setup() {
	setSize(config->gridSize.x - config->padding.x, config->sliderHeight * 6 + config->sliderTextHeight);
	for(int i=0; i<3; i++) {
		pct[i] = ofMap(getValue(i), 0, 255, 0.0, width);
		barwidth[i] = pct[i];
	}
}

void ofxSimpleGuiHexColorPicker::loadFromXML(ofxXmlSettings &XML) {
	for(int i=0; i<3; i++) {
		setValue(XML.getValue(controlType + "_" + key + ":values_" + ofToString(i), 0.0f), i);
	}
}

void ofxSimpleGuiHexColorPicker::saveToXML(ofxXmlSettings &XML) {
	printf("Saving to xml\n");
	XML.addTag(controlType + "_" + key);
	XML.pushTag(controlType + "_" + key);
	XML.addValue("name", name);
	for(int i=0; i<3; i++) {
		XML.addValue("values_" + ofToString(i), getValue(i));
	}
	XML.popTag();
}



float ofxSimpleGuiHexColorPicker::getValue(int i) {

	if(i==0) return (0x00FF0000 & *hexColor)>>16;
	else if(i==1) return  (0x0000FF00 & *hexColor)>>8;
	else if(i==2) return (0x000000FF & *hexColor);
	return 0;
}


void ofxSimpleGuiHexColorPicker::setValue(float f, int i) {
	int col[3];
	col[0] = getValue(0);
	col[1] = getValue(1);
	col[2] = getValue(2);
	col[i] = f;
	*hexColor = col[2] + (col[1]<<8) + (col[0]<<16);
}


void ofxSimpleGuiHexColorPicker::updateSlider() {
	if(!enabled) return;
	
	int i= (getMouseY() - y) / config->sliderHeight/2;
	if(i<0 || i>=3) return;
	
	if(pct[i] > width) {
		pct[i] = width;
	}
	else {
		pct[i] = getMouseX() - x;
		setValue(ofMap(pct[i], 0.0, (float)width, 0, 255), i);
	}
}

void ofxSimpleGuiHexColorPicker::onPress(int x, int y, int button) {
	updateSlider();
}

void ofxSimpleGuiHexColorPicker::onDragOver(int x, int y, int button) {
	updateSlider();
}

void ofxSimpleGuiHexColorPicker::onDragOutside(int x, int y, int button) {
	updateSlider();
}



//--------------------------------------------------------------------- update
void ofxSimpleGuiHexColorPicker::update() {
	if(!enabled) return;
	
	if(lock) {
		updateSlider();
	}
	
	//	enabled = false;
}

//--------------------------------------------------------------------- draw
void ofxSimpleGuiHexColorPicker::draw(float x, float y) {
	
	//	enabled = true;
	
	//update postion of gui object
	setPos(x, y);
	glPushMatrix();
	glTranslatef(x, y, 0);
	
	int startY = 0;
	for(int i=0; i<3; i++) {
		
		barwidth[i] = ofMap(getValue(i), 0, 255, 0.0, (float)width);
		if(barwidth[i] > width)	barwidth[i] = width;
		else if(barwidth[i] < 0) barwidth[i] = 0;
		
		ofEnableAlphaBlending();
		ofFill();
		setEmptyColor();
		ofRect(0, startY, width, config->sliderHeight*1.8);
		
		
		switch(i) {
			case 0:glColor3ub(getValue(i), 0, 0); break;
			case 1:glColor3ub(0, getValue(i), 0); break;
			case 2:glColor3ub(0, 0, getValue(i)); break;
		}
		
		ofRect(0, startY, barwidth[i], config->sliderHeight * 1.8);
		
		int iover = (getMouseY() - y) / config->sliderHeight/2;
		bool isOver = iover == i;
		if(isOver) {
			glColor3f(1, 1, 1);
		} else {
			glColor3f(0.5, 0.5, 0.5);
		}
		
		ofDrawBitmapString(ofToString(getValue(i), 4), 3, startY + 14);
		
		startY += config->sliderHeight * 2;
	}
	
	ofFill();
	
	setTextBGColor();
	ofRect(0, startY, width, config->sliderTextHeight);
	
	glColor3ub(getValue(0), getValue(1), getValue(2));
	//	ofRect(0, startY+config->sliderTextHeight, width, config->sliderTextHeight * 1.5);
	ofRect(150, startY + 3, width - 150 -3, config->sliderTextHeight - 8);
	
	setTextColor();
	string s = name;
	ofDrawBitmapString(s, 3, startY + 14);
	ofDisableAlphaBlending();
	glPopMatrix();
}
