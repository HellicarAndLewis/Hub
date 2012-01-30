/**
 * Instantiator.cpp
 * emptyExample
 *
 * Created by Marek Bereza on 29/06/2011.
 *
 */

#include "Instantiator.h"

xmlgui::Instantiator::Instantiator() {
	registerControls();
}

xmlgui::Control *xmlgui::Instantiator::createControl(string type) {
	controlConstructor c = registry[type];
	if(registry.find(type)!=registry.end()) {
		Control *cc = c();
		cc->type = type;
		return cc;
	} else {
		return new Control();
	}
}
