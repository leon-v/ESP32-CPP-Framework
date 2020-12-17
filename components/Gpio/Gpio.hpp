#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include <string>
#include <array>

using namespace std;

class Gpio{

	public:

	gpio_num_t pin;
	gpio_config_t config = {
		.pin_bit_mask = 0,
		.mode = GPIO_MODE_INPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE
	};

	string tag = "Gpio";
	bool taskRun = false;

	xQueueHandle eventQueue = NULL;

	static gpio_num_t pin0;
	static gpio_num_t pin1;
	static gpio_num_t pin2;
	static gpio_num_t pin3;
	static gpio_num_t pin4;
	static gpio_num_t pin5;
	static gpio_num_t pin6;
	static gpio_num_t pin7;
	static gpio_num_t pin8;
	static gpio_num_t pin9;
	static gpio_num_t pin10;
	static gpio_num_t pin11;
	static gpio_num_t pin12;
	static gpio_num_t pin13;
	static gpio_num_t pin14;
	static gpio_num_t pin15;
	static gpio_num_t pin16;
	static gpio_num_t pin17;
	static gpio_num_t pin18;
	static gpio_num_t pin19;

	static gpio_mode_t modeDisable;
	static gpio_mode_t modeInput;
	static gpio_mode_t modeOutput;
	static gpio_mode_t modeInputOutput;
	static gpio_mode_t modeOutputOpenDrain;
	static gpio_mode_t modeInputOutputOpenDrain;

	static gpio_int_type_t interruptTypeDisable;
	static gpio_int_type_t interruptTypeEdgePositive;
	static gpio_int_type_t interruptTypeEdgeNegative;
	static gpio_int_type_t interruptTypeEdgeAny;
	static gpio_int_type_t interruptTypeLevelLow;
	static gpio_int_type_t interruptTypeLevelHigh;

	Gpio(){
	}

	Gpio(gpio_num_t pin){

		this->setPin(pin);

		this->start();
	}

	Gpio(gpio_num_t pin, gpio_mode_t mode){

		this->setPin(pin);
		this->setMode(mode);

		this->start();
	}

	Gpio(gpio_num_t pin, gpio_mode_t mode, gpio_int_type_t interruptType){

		this->setPin(pin);
		this->setMode(mode);
		this->setInterruptType(interruptType);

		this->start();
	}

	virtual ~Gpio(){

		if (this->config.intr_type != GPIO_INTR_DISABLE) {

			gpio_isr_handler_remove(this->pin);

			this->taskRun = false;

			vQueueDelete(this->eventQueue);
		}

		ESP_LOGI(this->tag.c_str(), "Released");
	}

	virtual void isr(){
		if (this->getPin()) {
			ESP_LOGI(this->tag.c_str(), "High");
		}
		else{
			ESP_LOGI(this->tag.c_str(), "Low");
		}
	}

	static void staticIsr(void* vpGpio){

		Gpio* pGpio = (Gpio*) vpGpio;

		xQueueSendFromISR(pGpio->eventQueue, NULL, NULL);
	}

	void start(){

		ESP_LOGI(this->tag.c_str(), "Start");

		if (this->config.intr_type != GPIO_INTR_DISABLE) {

			ESP_LOGI(this->tag.c_str(), "Enabling ISR");

			this->eventQueue = xQueueCreate(1, 0);

			this->taskRun = true;
			xTaskCreate(Gpio::isrTaskDispatcher, "Gpio::isrTaskDispatcher", 2048, this, 10, NULL);

			gpio_install_isr_service(0);

			gpio_isr_handler_add(this->pin, this->staticIsr, this);

			xQueueSendFromISR(this->eventQueue, NULL, NULL);
		}

		ESP_ERROR_CHECK(gpio_config(&this->config));
	}

	static void isrTaskDispatcher(void* vpGpio){

		Gpio* pGpio = (Gpio*) vpGpio;

		ESP_LOGI(pGpio->tag.c_str(), "Task start");

		while(pGpio->taskRun) {

			if (!xQueueReceive(pGpio->eventQueue, NULL, portMAX_DELAY)) {
				continue;
			}

			pGpio->isr();
		}

		ESP_LOGI(pGpio->tag.c_str(), "Task end");

		vTaskDelete(NULL);
	}

	void setPin(gpio_num_t pin){

		this->pin = pin;

		this->config.pin_bit_mask = (1ULL << this->pin);

		this->tag.append(" ");
		this->tag += std::to_string(this->pin);
	}

	void setPinBitMask(uint64_t mask){
		this->config.pin_bit_mask = mask;
	}

	void setMode(gpio_mode_t mode){
		this->config.mode = mode;
	}

	void setModeDisable(){
		this->config.mode = GPIO_MODE_DISABLE;
	}
	
	void setModeInput(){
		this->config.mode = GPIO_MODE_INPUT;
	}

	void setModeOutput(){
		this->config.mode = GPIO_MODE_OUTPUT;
	}

	void setModeOutputOpenDrain(){
		this->config.mode = GPIO_MODE_OUTPUT_OD;
	}

	void setModeInputOutputOpenDrain(){
		this->config.mode = GPIO_MODE_INPUT_OUTPUT_OD;
	}

	void setModeInputOutput(){
		this->config.mode = GPIO_MODE_INPUT_OUTPUT;
	}

	void setPullUpEnable(bool enable) {
		this->config.pull_up_en = enable ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;
	}
	
	void setPullDownEnable(bool enable) {
		this->config.pull_down_en = enable ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE;
	}

	void setInterruptType(gpio_int_type_t type){
		this->config.intr_type = type;
		gpio_set_intr_type(this->pin, type);
	}

	void setInterruptDisable(){
		this->setInterruptType(GPIO_INTR_DISABLE);
	}

	void setInterruptPositiveEdge(){
		this->setInterruptType(GPIO_INTR_POSEDGE);
	}

	void setInterruptNegativeEdge(){
		this->setInterruptType(GPIO_INTR_NEGEDGE);
	}

	void setInterruptAnyEdge(){
		this->setInterruptType(GPIO_INTR_ANYEDGE);
	}
	
	void setInterruptLowLevel(){
		this->setInterruptType(GPIO_INTR_LOW_LEVEL);
	}
	
	void setInterruptHighLevel(){
		this->setInterruptType(GPIO_INTR_HIGH_LEVEL);
	}

	void high(){
		this->setPin(true);
	}

	void low(){
		this->setPin(false);
	}

	void setPin(bool level){
		ESP_ERROR_CHECK(gpio_set_level(this->pin, level ? 1 : 0));
	}

	bool getPin(){
		return gpio_get_level(this->pin) == 1;
	}
};

gpio_num_t Gpio::pin0 = GPIO_NUM_0;
gpio_num_t Gpio::pin1 = GPIO_NUM_1;
gpio_num_t Gpio::pin2 = GPIO_NUM_2;
gpio_num_t Gpio::pin3 = GPIO_NUM_3;
gpio_num_t Gpio::pin4 = GPIO_NUM_4;
gpio_num_t Gpio::pin5 = GPIO_NUM_5;
gpio_num_t Gpio::pin6 = GPIO_NUM_6;
gpio_num_t Gpio::pin7 = GPIO_NUM_7;
gpio_num_t Gpio::pin8 = GPIO_NUM_8;
gpio_num_t Gpio::pin9 = GPIO_NUM_9;
gpio_num_t Gpio::pin10 = GPIO_NUM_10;
gpio_num_t Gpio::pin11 = GPIO_NUM_11;
gpio_num_t Gpio::pin12 = GPIO_NUM_12;
gpio_num_t Gpio::pin13 = GPIO_NUM_13;
gpio_num_t Gpio::pin14 = GPIO_NUM_14;
gpio_num_t Gpio::pin15 = GPIO_NUM_15;
gpio_num_t Gpio::pin16 = GPIO_NUM_16;
gpio_num_t Gpio::pin17 = GPIO_NUM_17;
gpio_num_t Gpio::pin18 = GPIO_NUM_18;
gpio_num_t Gpio::pin19 = GPIO_NUM_19;

gpio_mode_t Gpio::modeDisable = GPIO_MODE_DISABLE;
gpio_mode_t Gpio::modeInput = GPIO_MODE_INPUT;
gpio_mode_t Gpio::modeOutput = GPIO_MODE_OUTPUT;
gpio_mode_t Gpio::modeInputOutput = GPIO_MODE_INPUT_OUTPUT;
gpio_mode_t Gpio::modeOutputOpenDrain = GPIO_MODE_INPUT_OUTPUT_OD;
gpio_mode_t Gpio::modeInputOutputOpenDrain = GPIO_MODE_INPUT_OUTPUT_OD;

gpio_int_type_t Gpio::interruptTypeDisable = GPIO_INTR_DISABLE;
gpio_int_type_t Gpio::interruptTypeEdgePositive = GPIO_INTR_POSEDGE;
gpio_int_type_t Gpio::interruptTypeEdgeNegative = GPIO_INTR_NEGEDGE;
gpio_int_type_t Gpio::interruptTypeEdgeAny = GPIO_INTR_ANYEDGE;
gpio_int_type_t Gpio::interruptTypeLevelLow = GPIO_INTR_LOW_LEVEL;
gpio_int_type_t Gpio::interruptTypeLevelHigh = GPIO_INTR_HIGH_LEVEL;