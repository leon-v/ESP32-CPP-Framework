#pragma once

#include "esp_log.h"

#include "HttpServer.hpp"
#include "Json.hpp"
#include <cJSON.h>

using namespace std;


class HttpApa102{

	public:
	string tag = "HttpApa102";
	Apa102Strip* apa102Strip;
	HttpServer* httpServer;

	class HttpApa102PostUri : public HttpServerUri{
		
		public:
		HttpApa102* httpApa102;

		HttpApa102PostUri(HttpApa102* httpApa102) : HttpServerUri("/leds", HTTP_POST) {
			this->httpApa102 = httpApa102;
		}

		void handler(httpd_req_t* pRequest) {

			httpd_resp_set_hdr(pRequest, "Access-Control-Allow-Origin", "*");

			string postBody = this->getPost(pRequest);

			cJSON* json;
			json = cJSON_Parse(postBody.c_str());
			
			if (!cJSON_IsArray(json)){
				this->respond(pRequest, "{\"success\":false, \"error\":\"Array Required\"}");
				return;
			}

			
			int index = -1;
			cJSON* item;
			cJSON* value;
			apa102_levels_t* led;
			cJSON_ArrayForEach(item, json) {

				index++;

				if (!cJSON_IsObject(item)) {
					continue;
				}

				value = cJSON_GetObjectItemCaseSensitive(item, "cl");
				if (cJSON_IsNumber(value)){
					if (value){
						this->httpApa102->apa102Strip->clear();
					}
				}

				value = cJSON_GetObjectItemCaseSensitive(item, "i");
				if (cJSON_IsNumber(value)){
					index = value->valueint;
				}

				led = this->httpApa102->apa102Strip->getLed(index);

				if (led == NULL){
					continue;
				}

				led->global = 3 | 0b11100000;

				value = cJSON_GetObjectItemCaseSensitive(item, "gl");
				if (cJSON_IsNumber(value)){
					led->global = value->valueint | 0b11100000;
				}

				value = cJSON_GetObjectItemCaseSensitive(item, "r");
				if (cJSON_IsNumber(value)){
					led->red = value->valueint;
				}

				value = cJSON_GetObjectItemCaseSensitive(item, "g");
				if (cJSON_IsNumber(value)){
					led->green = value->valueint;
				}

				value = cJSON_GetObjectItemCaseSensitive(item, "b");
				if (cJSON_IsNumber(value)){
					led->blue = value->valueint;
				}

				// ESP_LOGI("HA", "I: %d, R: %d, G: %d, B:%d", index, led->red, led->green, led->blue);
			}

			cJSON_Delete(json);

			this->httpApa102->apa102Strip->update();
			
			this->respond(pRequest, "{\"success\":true}");
		}
	};
	HttpApa102PostUri httpApa102PostUri;

	HttpApa102(Apa102Strip* apa102Strip, HttpServer* httpServer) : httpApa102PostUri(this){

		this->apa102Strip = apa102Strip;
		this->httpServer = httpServer;

		this->httpServer->registerUri(&this->httpApa102PostUri);
	}

};