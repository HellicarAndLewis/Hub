//
//  Timer.h
//  WailWall
//
//  Created by Joel Lewis on 21/02/2012.
//  Copyright (c) 2012 Hellicar & Lewis. All rights reserved.
//

#ifndef WailWall_Timer_h
#define WailWall_Timer_h

class Timer {
public:
	Timer() {
		reset();
		startTime = 0;
	}
	
	void start(float duration) {
		startTime = ofGetElapsedTimef();
		isRunning = true;
		this->duration = duration;

	}
	
	bool done() {
		return isRunning && ofGetElapsedTimef()>startTime+duration;
	}
	
	void reset() {
		isRunning = false;
	}
	
private:
	bool isRunning;
	float startTime;
	float duration;
};

#endif
