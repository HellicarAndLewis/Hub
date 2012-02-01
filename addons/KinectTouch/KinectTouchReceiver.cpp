/**
 *  KinectTouchReceiver.cpp
 *
 *  Created by Marek Bereza on 01/02/2012.
 */

#include "KinectTouchReceiver.h"



KinectTouchReceiver::KinectTouchReceiver() {
	this->listener = NULL;
}
void KinectTouchReceiver::setup(int port) {
	osc.setup(port);
}
void KinectTouchReceiver::setListener(KinectTouchListener *listener) {
	this->listener = listener;
}

void KinectTouchReceiver::update() {
	ofxOscMessage m;
	while(osc.getNextMessage(&m)) {
		// if there's no listener, continue to consume messages, but don't do anything about them
		if(listener==NULL) continue; 
		
		// otherwise deal with message
		//			printf("Address: %s\n", m.getAddress().c_str());
		if(m.getAddress()=="/touch/down") {
			int id = m.getArgAsInt32(0);
			blobs[id] = KinectTouch();
			blobs[id].x = m.getArgAsFloat(1);
			blobs[id].y = m.getArgAsFloat(2);
			blobs[id].z = m.getArgAsFloat(3);
			blobs[id].size = m.getArgAsFloat(4);
			blobs[id].id = id;
			
			listener->touchDown(blobs[id]);
			
		} else if(m.getAddress()=="/touch/moved") {
			int id = m.getArgAsInt32(0);
			bool found = true;
			if(blobs.find(id)==blobs.end()) {
				blobs[id] = KinectTouch();
				found = false;
			}
			ofVec3f p = ofVec3f(m.getArgAsFloat(1), m.getArgAsFloat(2), m.getArgAsFloat(3));
			blobs[id].vel = blobs[id] - p;
			blobs[id].x = p.x;
			blobs[id].y = p.y;
			blobs[id].z = p.z;
			blobs[id].size = m.getArgAsFloat(4);
			blobs[id].id = id;
			
			
			if(!found) {
				blobs[id].vel = 0;
				listener->touchDown(blobs[id]);
			} else {
				listener->touchMoved(blobs[id]);
			}
			//printf("%d %f %f %f\n", id, p.x, p.y, p.z);				
		} else if(m.getAddress()=="/touch/up") {
			int id = m.getArgAsInt32(0);
			if(blobs.find(id)!=blobs.end()) {
				listener->touchUp(blobs[id]);
				blobs.erase(id);
			}
		}
	}
}
