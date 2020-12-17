#pragma once

#include "esp_http_server.h"
#include "esp_log.h"

#include "HttpServerUri.hpp"

#include <string>
#include <vector>

using namespace std;

class HttpServer{

	public:

	string tag = "HttpServer";
	vector <HttpServerUri *> httpServerUris;

	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	HttpServer(){

		ESP_LOGI(this->tag.c_str(), "Construct starting");

		ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, this->eventDispatcher, this));
		ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, this->eventDispatcher, this));
		ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, this->eventDispatcher, this));

		ESP_LOGI(this->tag.c_str(), "Construct done");
	}

	HttpServer(uint16_t port){
		this->config.server_port = port;
		this->tag.append(":");
		this->tag += std::to_string(port);
	}

	void start() {

		ESP_LOGI(this->tag.c_str(), "Starting server on port: '%d'", this->config.server_port);

		if (this->server) {
			ESP_LOGI(this->tag.c_str(), "Server already running");
			return;
		}

		ESP_ERROR_CHECK_WITHOUT_ABORT(httpd_start(&this->server, &this->config));

		for (HttpServerUri* pHttpServerUri: this->httpServerUris){
			ESP_LOGI(this->tag.c_str(), "Registering URI: %s, Method:%d", pHttpServerUri->uri.uri, pHttpServerUri->uri.method);

			ESP_ERROR_CHECK_WITHOUT_ABORT(httpd_register_uri_handler(this->server, &(pHttpServerUri->uri)));
		}
	}

	void stop(){

		ESP_LOGI(this->tag.c_str(), "Stopping");

		if (!(this->server)) {
			ESP_LOGI(this->tag.c_str(), "Not running");
			return;
		}
		
        ESP_ERROR_CHECK_WITHOUT_ABORT(httpd_stop(this->server));

        this->server = NULL;

		ESP_LOGI(this->tag.c_str(), "Stopped");
	}

	void networkConnected(){
		this->start();
	}

	void networkDisconnected(){
		this->stop();
	}

	static void eventDispatcher(void* vpHttpServer, esp_event_base_t eventBase, int32_t eventId, void* eventData) {
		HttpServer* pHttpServer = (HttpServer*) vpHttpServer;
		pHttpServer->eventHandler(eventBase, eventId, eventData);
	}

	void eventHandler(esp_event_base_t eventBase, int32_t eventId, void* eventData) {
	
		if (eventBase == IP_EVENT) {

			if (eventId == IP_EVENT_STA_GOT_IP) {
				this->networkConnected();
			}

			else if (eventId == IP_EVENT_ETH_GOT_IP) {
				this->networkConnected();
			}
		}

		else if (eventBase == WIFI_EVENT) {

			if (eventId == WIFI_EVENT_AP_START) {
				this->networkConnected();
			}

			else if (eventId == WIFI_EVENT_STA_DISCONNECTED) {
				this->networkDisconnected();
			}
		}

		else if (eventBase == ETH_EVENT) {
			if (eventId == ETHERNET_EVENT_DISCONNECTED) {
				this->networkDisconnected();
			}
		}
	}

	void registerUri(HttpServerUri* pHttpServerUri) {
		ESP_LOGI(this->tag.c_str(), "Adding URI: %s", pHttpServerUri->path.c_str());
		this->httpServerUris.push_back(pHttpServerUri);
	}
};