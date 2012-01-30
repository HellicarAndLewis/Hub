/**
 * Instantiator.h
 * emptyExample
 *
 * Created by Marek Bereza on 29/06/2011.
 *
 */
#pragma once

#include "ofMain.h"
#include "Control.h"
#define INSTANTIATE(A) xmlgui::Instantiator::getInstance()->createControl(A)
typedef xmlgui::Control* (*controlConstructor)();

namespace xmlgui {
	template<class T> 
	xmlgui::Control *ControlInstantiator() {
		return new T();
	}
	
	
	
	
	
	class Instantiator: public vector<string> {
	public:
		static Instantiator *getInstance() {
			static Instantiator *instance = NULL;
			if(instance==NULL) {
				instance = new Instantiator();
			}
			return instance;
		}
		Control *createControl(string type);
		
	private:

		
		Instantiator();
		void registerControls();
		
		map<string,controlConstructor> registry;
		
		void addControlConstructor(controlConstructor constructor, string type) {
			registry[type] = constructor;
			push_back(type);
		}
		
		
	};
};