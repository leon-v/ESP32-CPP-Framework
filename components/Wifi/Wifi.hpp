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

#include "WifiClient.hpp"
#include "WifiAp.hpp"

#include <string>

using namespace std;

#define EXAMPLE_ESP_MAXIMUM_RETRY 10

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT	BIT0
#define WIFI_FAIL_BIT		BIT1

class Wifi{

	public:
	string tag = "Wifi";

	wifi_init_config_t config;
	int retryCount = 0;

	class WifiInit {

		public:
		string tag = "Wifi Init";

		WifiInit(Wifi* wifi) {

			ESP_LOGI(this->tag.c_str(), "Construct starting");

			wifi->config = WIFI_INIT_CONFIG_DEFAULT();
			ESP_ERROR_CHECK(esp_wifi_init(&wifi->config));

			ESP_LOGI(this->tag.c_str(), "Construct done");
		}
	};

	WifiInit wifiInit;
	WifiClient client;
	WifiAp ap;

	Wifi():
		wifiInit(this),
		client(),
		ap()
		{
		ESP_LOGI(this->tag.c_str(), "Construct starting");

		ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, this->eventDispatcher, this));
		ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, this->eventDispatcher, this));
		
		ESP_LOGI(this->tag.c_str(), "Construct done");
	}

	static void eventDispatcher(void* vpWifi, esp_event_base_t eventBase, int32_t eventId, void* eventData) {
		Wifi* pWifi = (Wifi*) vpWifi;
		pWifi->eventHandler(eventBase, eventId, eventData);
	}

	void eventHandler(esp_event_base_t eventBase, int32_t eventId, void* eventData) {

		if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_START) {
			esp_wifi_connect();
		}

		else if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_DISCONNECTED) {

			if (this->retryCount < EXAMPLE_ESP_MAXIMUM_RETRY) {
				esp_wifi_connect();
				this->retryCount++;
				ESP_LOGI(this->tag.c_str(), "retry to connect to the AP");
			}
			else {
				// xEventGroupSetBits(this->eventGroup, WIFI_FAIL_BIT);
			}
			ESP_LOGI(this->tag.c_str(),"connect to the AP fail");
		}

		else if (eventBase == IP_EVENT && eventId == IP_EVENT_STA_GOT_IP) {
			
			ip_event_got_ip_t* event = (ip_event_got_ip_t*) eventData;

			ESP_LOGI(this->tag.c_str(), "got ip:" IPSTR, IP2STR(&event->ip_info.ip));

			this->retryCount = 0;

			// xEventGroupSetBits(this->eventGroup, WIFI_CONNECTED_BIT);
		}
	}

	static string getAuthModeKey(wifi_auth_mode_t authMode) {

		switch (authMode){
			case WIFI_AUTH_OPEN:
				return "OPEN";
			case WIFI_AUTH_WEP:
				return "WEP";
			case WIFI_AUTH_WPA_PSK:
				return "WPA_PSK";
			case WIFI_AUTH_WPA2_PSK:
				return "WPA2_PSK";
			case WIFI_AUTH_WPA_WPA2_PSK:
				return "WPA_WPA2_PSK";
			case WIFI_AUTH_WPA2_ENTERPRISE:
				return "WPA2_ENTERPRISE";
			case WIFI_AUTH_WPA3_PSK:
				return "WPA3_PSK";
			case WIFI_AUTH_WPA2_WPA3_PSK:
				return "WPA2_WPA3_PSK";
			case WIFI_AUTH_MAX:
				return "MAX";
		}

		return "";
	}

	static wifi_auth_mode_t getAuthModeFromKey(string key) {

		if (key.compare("OPEN") == 0) {
			return WIFI_AUTH_OPEN;
		}
		else if (key.compare("WEP") == 0) {
			return WIFI_AUTH_WEP;
		}
		else if (key.compare("WPA_PSK") == 0) {
			return WIFI_AUTH_WPA_PSK;
		}
		else if (key.compare("WPA2_PSK") == 0) {
			return WIFI_AUTH_WPA2_PSK;
		}
		else if (key.compare("WPA_WPA2_PSK") == 0) {
			return WIFI_AUTH_WPA_WPA2_PSK;
		}
		else if (key.compare("WPA2_ENTERPRISE") == 0) {
			return WIFI_AUTH_WPA2_ENTERPRISE;
		}
		else if (key.compare("WPA3_PSK")) {
			return WIFI_AUTH_WPA3_PSK;
		}
		else if (key.compare("WPA2_WPA3_PSK") == 0) {
			return WIFI_AUTH_WPA2_WPA3_PSK;
		}
		else if (key.compare("MAX") == 0) {
			return WIFI_AUTH_MAX;
		}

		return WIFI_AUTH_OPEN;
	}
};