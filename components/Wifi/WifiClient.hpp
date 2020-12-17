#pragma once

#include "esp_log.h"

#include <string>
#include <cstring>
#include <vector>

using namespace std;

#define DEFAULT_SCAN_LIST_SIZE 8

class WifiClientScannedAp{

	public:

	string ssid;
	int rssi;
	int channel;
	wifi_auth_mode_t authMode;

	WifiClientScannedAp(char * cpSsid, int rssi, int channel, wifi_auth_mode_t authMode){
		this->ssid = string(cpSsid);
		this->rssi = rssi;
		this->channel = channel;
		this->authMode = authMode;
		
	}
};

class WifiClient{

	public:

	string tag = "WifiClient";
	wifi_config_t config;

	WifiClient(){
		ESP_LOGI(this->tag.c_str(), "Construct starting");

		ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, this->eventDispatcher, this));

		ESP_LOGI(this->tag.c_str(), "Construct done");
	}

	void start(string ssid, string password, wifi_auth_mode_t authMode){

		strcpy( (char*) &this->config.sta.ssid, ssid.c_str());
		strcpy( (char*) &this->config.sta.password, password.c_str());

		this->config.sta.threshold.authmode = authMode;

		this->start();

		ESP_ERROR_CHECK(esp_wifi_connect());
	}
	void start(){

		this->config.sta.pmf_cfg.capable = true;
		this->config.sta.pmf_cfg.required = false;
		
		ESP_LOGI(this->tag.c_str(), "Start starting");

		esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    	assert(sta_netif);

		ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

		ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &this->config));

    	ESP_ERROR_CHECK(esp_wifi_start());

		ESP_LOGI(this->tag.c_str(), "Start done");
	}


	vector <WifiClientScannedAp> getScan(){

		vector <WifiClientScannedAp> wifiClientScannedAps;

		uint16_t number = DEFAULT_SCAN_LIST_SIZE;
		wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
		uint16_t ap_count = 0;
		memset(ap_info, 0, sizeof(ap_info));
		
		ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));

		ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));

		ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));

		ESP_LOGI(this->tag.c_str(), "Total APs scanned = %u", ap_count);

		for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {

			WifiClientScannedAp wifiClientScannedAp(
				(char*) &ap_info[i].ssid,
				ap_info[i].rssi,
				ap_info[i].primary,
				ap_info[i].authmode
			);

			wifiClientScannedAps.push_back(wifiClientScannedAp);
		}

		return wifiClientScannedAps;
	}

	static void eventDispatcher(void* vpWifiClient, esp_event_base_t eventBase, int32_t eventId, void* eventData) {
		WifiClient* pWifiClient = (WifiClient*) vpWifiClient;
		pWifiClient->eventHandler(eventBase, eventId, eventData);
	}

	void eventHandler(esp_event_base_t eventBase, int32_t eventId, void* eventData) {

		
	}
};