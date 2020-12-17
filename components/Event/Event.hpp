#pragma once

#include <string>

using namespace std;

class Event{
	public:
	string tagh = "Event";
	EventGroupHandle_t group;
	Events(){

		this->group = xEventGroupCreate();
		
		

		ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, Event::handler, this));
		ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, Event::handler, this));
		ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, Event::handler, this));
		ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, Event::handler, this));
	}

	static void handler(void* vpEvent, esp_event_base_t event_base, int32_t event_id, void* event_data) {

		Event* pEvent = (Event*) vpEvent;

		ESP_LOGI(pEvent->tag.c_str(), "Event. %s", pEvent->getEventIdName(event_base, event_id).c_str());

		// main->wifi.eventHandler(event_base, event_id, event_data);
		
		// main->httpServer.eventHandler(event_base, event_id, event_data);
	}

	void registerHandler(){

	}

	static string getEventIdName(esp_event_base_t eventBase, int32_t eventId) {

		string eventName = "";

		if (eventBase == WIFI_EVENT) {

			eventName.append("WIFI_EVENT");

			switch (eventId) {
				case WIFI_EVENT_WIFI_READY:
					eventName.append(" : WIFI_EVENT_WIFI_READY");
				break;
				case WIFI_EVENT_SCAN_DONE:
					eventName.append(" : WIFI_EVENT_SCAN_DONE");
				break;
				case WIFI_EVENT_STA_START:
					eventName.append(" : WIFI_EVENT_STA_START");
				break;
				case WIFI_EVENT_STA_STOP:
					eventName.append(" : WIFI_EVENT_STA_STOP");
				break;
				case WIFI_EVENT_STA_CONNECTED:
					eventName.append(" : WIFI_EVENT_STA_CONNECTED");
				break;
				case WIFI_EVENT_STA_DISCONNECTED:
					eventName.append(" : WIFI_EVENT_STA_DISCONNECTED");
				break;
				case WIFI_EVENT_STA_AUTHMODE_CHANGE:
					eventName.append(" : WIFI_EVENT_STA_AUTHMODE_CHANGE");
				break;
				case WIFI_EVENT_STA_WPS_ER_SUCCESS:
					eventName.append(" : WIFI_EVENT_STA_WPS_ER_SUCCESS");
				break;
				case WIFI_EVENT_STA_WPS_ER_FAILED:
					eventName.append(" : WIFI_EVENT_STA_WPS_ER_FAILED");
				break;
				case WIFI_EVENT_STA_WPS_ER_TIMEOUT:
					eventName.append(" : WIFI_EVENT_STA_WPS_ER_TIMEOUT");
				break;
				case WIFI_EVENT_STA_WPS_ER_PIN:
					eventName.append(" : WIFI_EVENT_STA_WPS_ER_PIN");
				break;
				case WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP:
					eventName.append(" : WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP");
				break;
				case WIFI_EVENT_AP_START:
					eventName.append(" : WIFI_EVENT_AP_START");
				break;
				case WIFI_EVENT_AP_STOP:
					eventName.append(" : WIFI_EVENT_AP_STOP");
				break;
				case WIFI_EVENT_AP_STACONNECTED:
					eventName.append(" : WIFI_EVENT_AP_STACONNECTED");
				break;
				case WIFI_EVENT_AP_STADISCONNECTED:
					eventName.append(" : WIFI_EVENT_AP_STADISCONNECTED");
				break;
				case WIFI_EVENT_AP_PROBEREQRECVED:
					eventName.append(" : WIFI_EVENT_AP_PROBEREQRECVED");
				break;
				default:
					eventName.append(" : UNHANDELED");
				break;
			}
		}

		else if (eventBase == IP_EVENT) {

			eventName.append("IP_EVENT");

			switch (eventId) {
				case IP_EVENT_STA_GOT_IP:
					eventName.append(" : IP_EVENT_STA_GOT_IP");
				break;
				case IP_EVENT_STA_LOST_IP:
					eventName.append(" : IP_EVENT_STA_LOST_IP");
				break;
				case IP_EVENT_AP_STAIPASSIGNED:
					eventName.append(" : IP_EVENT_AP_STAIPASSIGNED");
				break;
				case IP_EVENT_GOT_IP6:
					eventName.append(" : IP_EVENT_GOT_IP6");
				break;
				case IP_EVENT_ETH_GOT_IP:
					eventName.append(" : IP_EVENT_ETH_GOT_IP");
				break;
				case IP_EVENT_PPP_GOT_IP:
					eventName.append(" : IP_EVENT_PPP_GOT_IP");
				break;
				case IP_EVENT_PPP_LOST_IP:
					eventName.append(" : IP_EVENT_PPP_LOST_IP");
				break;
			}
		}

		else if (eventBase == ETH_EVENT) {

			eventName.append("ETH_EVENT");

			switch (eventId) {
				case ETHERNET_EVENT_START:
					eventName.append(" : ETHERNET_EVENT_START");
				break;
				case ETHERNET_EVENT_STOP:
					eventName.append(" : ETHERNET_EVENT_STOP");
				break;
				case ETHERNET_EVENT_CONNECTED:
					eventName.append(" : ETHERNET_EVENT_CONNECTED");
				break;
				case ETHERNET_EVENT_DISCONNECTED:
					eventName.append(" : ETHERNET_EVENT_DISCONNECTED");
				break;
				default:
					eventName.append(" : UNHANDELED");
				break;
			}
		}
		else{
			eventName.append(" : UNHANDELED");
		}

		return eventName;
	}
}