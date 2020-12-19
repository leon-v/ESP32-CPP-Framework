#pragma once

#include "esp_log.h"

using namespace std;

#define DMA_CHAN    2

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22

class SpiConfig {

	public:
	spi_bus_config_t busConfiguration ={
		.mosi_io_num = PIN_NUM_MOSI,
		.miso_io_num = PIN_NUM_MISO,
		.sclk_io_num = PIN_NUM_CLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = 4094,
		.flags = 0,
		.intr_flags = 0
	};

	spi_device_interface_config_t deviceConfiguration ={
		.command_bits = 0,
		.address_bits = 0,
		.dummy_bits = 0,
		.mode = 0,                                	//SPI mode 0
		.duty_cycle_pos = 128,
		.cs_ena_pretrans = 16,
		.cs_ena_posttrans = 16,
		.clock_speed_hz = 10*1000*1000,           	//Clock out at 10 MHz
		.input_delay_ns = 0,
		.spics_io_num = PIN_NUM_CS,               	//CS pin
		.flags = 0,
		.queue_size = 1,                          	//We want to be able to queue 7 transactions at a time
		.pre_cb = SpiConfig::preTransferCallback,	//Specify pre-transfer callback to handle D/C line
		.post_cb = SpiConfig::postTransferCallback
	};

	static void preTransferCallback(spi_transaction_t* transaction){

	};

	static void postTransferCallback(spi_transaction_t* transaction){

	};
};

class Spi{

	public:

	SpiConfig config;
	spi_device_handle_t device;
	int dmaChannel = 1;
	spi_transaction_t transaction;

	Spi(SpiConfig config){
		this->config = config;

		Spi();
	}

	Spi(){
		ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &this->config.busConfiguration, this->dmaChannel));

		ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &this->config.deviceConfiguration, &this->device));
	}

	void transfer(uint8_t* sendBuffer, size_t length){

		if (length <= 0){
			return;
		}

		memset(&this->transaction, 0, sizeof(this->transaction)); 
		
		this->transaction.length = length * 8;

		this->transaction.tx_buffer = sendBuffer;
		this->transaction.rx_buffer = sendBuffer;

		//Still not sure what D/C is
		this->transaction.user = (void*) 1; //D/C needs to be set to 1

		ESP_ERROR_CHECK(spi_device_polling_transmit(this->device, &this->transaction));
	}
};