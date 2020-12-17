#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include <string>
#include <cstring>

using namespace std;

class WifiAp{

	public:

	string tag = "WifiAp";
	wifi_config_t config = {};

	WifiAp(){
		ESP_LOGI(this->tag.c_str(), "Construct starting");

		ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, this->eventDispatcher, this));

		ESP_LOGI(this->tag.c_str(), "Construct done");
	}

	void start() {

		ESP_LOGI(this->tag.c_str(), "Start starting");
		
		esp_netif_create_default_wifi_ap();

		char * configSsid = (char *) &this->config.ap.ssid;
		char * configPassword = (char *) &this->config.ap.password;

		strcpy(configSsid, CONFIG_ESP_WIFI_SSID);
		this->config.ap.ssid_len = strlen(CONFIG_ESP_WIFI_SSID);
		strcpy(configPassword, CONFIG_ESP_WIFI_PASSWORD);

		this->config.ap.channel = CONFIG_ESP_WIFI_CHANNEL;
        this->config.ap.max_connection = CONFIG_ESP_MAX_STA_CONN;
        this->config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;

		if (strlen( (char *) &this->config.ap.password) == 0) {
			this->config.ap.authmode = WIFI_AUTH_OPEN;
		}

		ESP_LOGI(this->tag.c_str(), "wifi_init_softap set. SSID:%s password:%s channel:%d", (char *) &this->config.ap.ssid, (char *) &this->config.ap.password, CONFIG_ESP_WIFI_CHANNEL);

		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

		ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &this->config));

		ESP_ERROR_CHECK(esp_wifi_start());

		ESP_LOGI(this->tag.c_str(), "Start done");
	}

	static void eventDispatcher(void* vpWifiAp, esp_event_base_t eventBase, int32_t eventId, void* eventData) {
		WifiAp* pWifiAp = (WifiAp*) vpWifiAp;
		pWifiAp->eventHandler(eventBase, eventId, eventData);
	}
	
	void eventHandler(esp_event_base_t eventBase, int32_t eventId, void* eventData) {

		if (eventBase == WIFI_EVENT) {
			if (eventId == WIFI_EVENT_AP_STACONNECTED) {

				wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) eventData;
				ESP_LOGI(this->tag.c_str(), "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);

			}
			
			else if (eventId == WIFI_EVENT_AP_STADISCONNECTED) {

				wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) eventData;
				ESP_LOGI(this->tag.c_str(), "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
			}
		}
	}
};