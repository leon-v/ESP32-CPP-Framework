#pragma once

#include "esp_log.h"

#include "Spi.hpp"

#include <vector>

using namespace std;

struct apa102_levels_t{
	uint8_t global;
	uint8_t blue;
	uint8_t green;
	uint8_t red;
};

class Apa102Strip{
	public:
	
	string tag = "Apa102Strip";
	Spi* spi = NULL;

	size_t length = 0;
	size_t spiBufferLength = 0;
	apa102_levels_t* spiBuffer;
	apa102_levels_t* ledBuffer;
	
	Apa102Strip(size_t length){

		this->length = length;
		this->spiBufferLength = (this->length + 2) * sizeof(apa102_levels_t);

		this->spiBuffer = (apa102_levels_t*) heap_caps_malloc(this->spiBufferLength, MALLOC_CAP_8BIT);

		assert(this->spiBuffer != NULL);

		this->ledBuffer =  &this->spiBuffer[1];

		this->clear();

		this->initSpi();

		this->update();
		this->update();
	}

	~Apa102Strip(){
		heap_caps_free(this->spiBuffer);
	}

	void clear(){

		memset(this->spiBuffer, 0, this->spiBufferLength);

		for (int i = 0; i < this->length; i++){
			this->setLedGlobal(i, 5);
		}
	}

	void setLed(int index, uint8_t global, uint8_t red, uint8_t green, uint8_t blue) {

		if (index >= this->length){
			ESP_LOGE(this->tag.c_str(), "Index %d higer than string length %d", index, this->length);
			return;
		}

		apa102_levels_t* apa102 = &this->ledBuffer[index];

		apa102->global = global | 0b11100000;
		apa102->red = red;
		apa102->green = green;
		apa102->blue = blue;
	}

	void setLedGlobal(int index, uint8_t value) {

		if (index >= this->length){
			ESP_LOGE(this->tag.c_str(), "Index %d higer than string length %d", index, this->length);
			return;
		}

		this->ledBuffer[index].global = value | 0b11100000;
	}

	void setLedRed(int index, uint8_t value) {

		if (index >= this->length){
			ESP_LOGE(this->tag.c_str(), "Index %d higer than string length %d", index, this->length);
			return;
		}

		this->ledBuffer[index].red = value;
	}

	void setLedGreen(int index, uint8_t value) {

		if (index >= this->length){
			ESP_LOGE(this->tag.c_str(), "Index %d higer than string length %d", index, this->length);
			return;
		}

		this->ledBuffer[index].green = value;
	}

	void setLedBlue(int index, uint8_t value) {

		if (index >= this->length){
			ESP_LOGE(this->tag.c_str(), "Index %d higer than string length %d", index, this->length);
			return;
		}

		this->ledBuffer[index].blue = value;
	}

	apa102_levels_t* getLed(int index){

		if (index >= this->length){
			ESP_LOGE(this->tag.c_str(), "Index %d higer than string length %d", index, this->length);
			return NULL;
		}

		if (index < 0){
			ESP_LOGE(this->tag.c_str(), "Index %d must be > 0", index);
			return NULL;
		}

		return &this->ledBuffer[index];
	}

	apa102_levels_t* getString(){
		return this->ledBuffer;
	}

	void initSpi(){

		if (!this->spi) {
			static Spi* spi = new Spi;
			this->spi = spi;
		}

		this->spi->setTxBuffer(this->spiBuffer);

		this->spi->setLengthBytes(this->spiBufferLength);
	}

	void update(){

		// ESP_LOGI(this->tag.c_str(), "Getting result");
		this->spi->getResult();

		// ESP_LOGI(this->tag.c_str(), "Queuing update");
		this->spi->queue();
	}

	// void shift(int amount){

	// 	apa102_levels_t* newBuffer = (apa102_levels_t*) heap_caps_malloc(this->length * sizeof(apa102_levels_t), MALLOC_CAP_8BIT);

	// 	if (newBuffer == NULL) {
	// 		ESP_LOGE(this->tag.c_str(), "Failed to allocate memory for shift");
	// 		return;
	// 	}

	// 	for (int from = 0; from < this->length; from++) {

	// 		int to = from + amount;

	// 		while(to >= this->length){
	// 			to -= this->length;
	// 		}
			
	// 		while(to < 0) {
	// 			to += this->length;
	// 		}
			
	// 		// ESP_LOGW(this->tag.c_str(), "From: %d, To: %d", from, to);
	// 		memcpy(&newBuffer[to], &this->buffer[from], sizeof(apa102_levels_t));
	// 	}

	// 	memcpy(this->buffer, newBuffer, this->length * sizeof(apa102_levels_t));

	// 	heap_caps_free(newBuffer);
	// }
};