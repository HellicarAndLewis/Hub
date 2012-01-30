/**
 * GuiDrawable.h
 * KinectTracker
 *
 * Created by Marek Bereza on 08/02/2011.
 *
 */
#include "Control.h"
#pragma once
namespace xmlgui {
	class Drawable: public xmlgui::Control {
	public:
		ofBaseDraws *drawable;
		Drawable(): Control() {
			drawable = NULL;
		}
		
		void draw() {
			if(drawable!=NULL) {
				//ofVec2f p = getAbsolutePosition();
				//glPushMatrix();
				//glTranslatef(-p.x, -p.y, 0);
				drawable->draw(x, y, width, height);
				//glPopMatrix();
			} else {
				ofNoFill();
				ofSetHexColor(0xFFFFFF);
				ofRect(*this);
				ofFill();
			}
		}
	};
}