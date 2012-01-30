#pragma once
#include "Container.h"
#include "Instantiator.h"
#include "Slider.h"
#include "ofGuiEventDispatcher.h"
namespace xmlgui {
	class SimpleGui: public xmlgui::Container {
	public:
		void setup() {
			events.setup(this);
			setLayoutType(xmlgui::LayoutType_vertical);
			enabled = false;
			setEnabled(true);
			setEnabled(false);
			x = 10;
			y = 20;
		}
		void setEnabled(bool enabled) {
			if(this->enabled!=enabled) {
				events.setEnabled(enabled);
				this->enabled = enabled;
			}
		}
		void addSlider(string name, float &value, float min, float max) {
			using namespace xmlgui;
			Slider *slider = (Slider*)INSTANTIATE("slider");
			slider->name = name;
			slider->pointToValue(&value);
			slider->min = min;
			slider->max = max;
			slider->width = 200;
			slider->showValue = true;
			addChild(slider);

		}
		xmlgui::ofGuiEventDispatcher events;
		
		bool enabled;

		void toggle() {
			setEnabled(!enabled);
		}
	};
};