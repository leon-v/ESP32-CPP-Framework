#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"

#include <string>
#include <stdlib.h>

#include <cstring>

using namespace std;

#define ESPTOUCH_DONE_BIT	BIT2

class WifiSmartConfig{

	public:
	EventGroupHandle_t eventGroup;

	string tag = "WifiSmartConfig";

	esp_netif_t *stationNetIf;

	WifiSmartConfig(EventGroupHandle_t eventGroup){
		this->eventGroup = eventGroup;
	}

	void start(){

		ESP_LOGI(this->tag.c_str(), "Starting");
		
		this->stationNetIf = esp_netif_create_default_wifi_sta();
    	assert(this->stationNetIf);

		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    	ESP_ERROR_CHECK(esp_wifi_start());

		ESP_LOGI(this->tag.c_str(), "Started");
	}

	void eventHandler(esp_event_base_t event_base, int32_t event_id, void* event_data) {

		if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {

        	ESP_LOGI(this->tag.c_str(), "Scan done");
    	}
	
		else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
			ESP_LOGI(this->tag.c_str(), "Found channel");
		}
	
		else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
			ESP_LOGI(this->tag.c_str(), "Got SSID and password");

			smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
			wifi_config_t wifi_config;
			uint8_t ssid[33] = { 0 };
			uint8_t password[65] = { 0 };

			bzero(&wifi_config, sizeof(wifi_config_t));
			memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
			memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
			wifi_config.sta.bssid_set = evt->bssid_set;
			if (wifi_config.sta.bssid_set == true) {
				memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
			}

			memcpy(ssid, evt->ssid, sizeof(evt->ssid));
			memcpy(password, evt->password, sizeof(evt->password));
			ESP_LOGI(this->tag.c_str(), "SSID:%s", ssid);
			ESP_LOGI(this->tag.c_str(), "PASSWORD:%s", password);

			ESP_ERROR_CHECK( esp_wifi_disconnect() );
			ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
			ESP_ERROR_CHECK( esp_wifi_connect() );
		} else if (event_base == SC_EVENT && event_id == SC_EVENT_SEND_ACK_DONE) {
			xEventGroupSetBits(this->eventGroup, ESPTOUCH_DONE_BIT);
		}
	}
};