#pragma once

#include "esp_log.h"

#include "Spi.hpp"

#include <vector>

using namespace std;

struct apa102_levels_t{
	uint8_t global;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

class Apa102Strip{
	public:
	
	string tag = "Apa102Strip";
	Spi* spi = NULL;

	size_t length = 0;
	apa102_levels_t* buffer;
	
	Apa102Strip(size_t length){

		this->length = length;

		this->buffer = (apa102_levels_t*) heap_caps_malloc(this->length * sizeof(apa102_levels_t), MALLOC_CAP_8BIT);

		this->initSpi();
	}

	void setLed(int index, uint8_t global, uint8_t red, uint8_t green, uint8_t blue) {

		if (index >= this->length){
			ESP_LOGE(this->tag.c_str(), "Index %d higer than string length %d", index, this->length);
			return;
		}

		apa102_levels_t* apa102 = &this->buffer[index];

		apa102->global = global;
		apa102->red = red;
		apa102->green = green;
		apa102->blue = blue;
	}

	void setLedGlobal(int index, uint8_t value) {
		this->buffer[index].global = value;
	}

	void setLedRed(int index, uint8_t value) {
		this->buffer[index].red = value;
	}

	void setLedGreen(int index, uint8_t value) {
		this->buffer[index].green = value;
	}

	void setLedBlue(int index, uint8_t value) {
		this->buffer[index].blue = value;
	}

	apa102_levels_t* getLed(int index){
		return &this->buffer[index];
	}

	apa102_levels_t* getString(){
		return this->buffer;
	}

	void initSpi(){

		if (!this->spi) {
			static Spi* spi = new Spi;
			this->spi = spi;
		}

		this->spi->setTxBuffer(this->buffer);

		this->spi->setLengthBytes(this->length);
	}

	void update(){
		this->spi->queue();
	}
};