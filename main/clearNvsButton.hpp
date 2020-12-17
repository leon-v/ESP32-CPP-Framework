#pragma once

#include "Gpio.hpp"
#include "Nvs.hpp"

class ClearNvsButton: public Gpio{
	public:
	bool erased = false;
	ClearNvsButton() :
		Gpio(
			Gpio::pin0,
			Gpio::modeInput,
			Gpio::interruptTypeEdgeNegative
	){}
	void isr(){

		if (erased){
			return;
		}

		ESP_LOGW(this->tag.c_str(), "Erasing NVS");

		Nvs::reset();

		erased = true;
	}
};