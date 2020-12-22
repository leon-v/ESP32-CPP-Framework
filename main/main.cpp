#include "esp_log.h"

#include "Exceptions.hpp"
#include "Nvs.hpp"
#include "Wifi.hpp"
#include "HttpServer.hpp"

#include "HttpWifiConfig.hpp"
#include "clearNvsButton.hpp"
#include "Apa102.hpp"
#include "HttpApa102.hpp"

#include <string>

using namespace std;

class Main{

	public:

	string tag = "Main";

	

	class MainInit{
		public:
		MainInit (Main* main){

			ESP_ERROR_CHECK(esp_netif_init());

			ESP_ERROR_CHECK(esp_event_loop_create_default());
		}
	};

	MainInit mainInit;

	// Event event;
	Nvs nvs;
	Wifi wifi;
	HttpServer httpServer;

	Main():
		mainInit(this),
		wifi(),
		httpServer()
		{

		ESP_LOGI(this->tag.c_str(), "Example start");

		this->powerUpFactoryReset();

		static Nvs wifiConfig = Nvs("wifi");

		string wifiSsid = wifiConfig.getString("ssid");
		string wifiPassword = wifiConfig.getString("password");
		int wifiAuthMode = wifiConfig.getInt("authMode");

		if (wifiSsid.length() <= 0) {
			static HttpWifiConfig httpWifiConfig(&this->httpServer, &this->wifi, &wifiConfig);
		}
		else{
			this->wifi.client.start(wifiSsid, wifiPassword, (wifi_auth_mode_t) wifiAuthMode);
		}

		int ledCount = 60;
		
		static Apa102Strip apa102Strip(ledCount);

		static HttpApa102 httpApa102(&apa102Strip, &this->httpServer);

		// int counter = 0;
		// while (true){

		// 	uint8_t data[] = {0xAA};
		// 	ESP_LOGI(this->tag.c_str(), "TX %02x", data[0]);
		// 	this->apa102Spi.transfer(data, 1);
		// 	ESP_LOGI(this->tag.c_str(), "RX %02x", data[0]);

			// apa102_levels_t* led;

			// for (int i = 0; i < ledCount; i++){
			// 	led = apa102Strip.getLed(i);

			// 	if (!led){
			// 		continue;
			// 	}
				
			// 	led->global = 64;
			// 	led->red = (1.03 * counter * i);
			// 	led->green = (1.01 * counter * i);
			// 	led->blue = counter * i;
			// }
			

			// apa102Strip.update();

			// vTaskDelay(30 / portTICK_RATE_MS);

			// if ((counter % 10) == 0) {
			// 	ESP_LOGW(this->tag.c_str(), "Shift");
			// 	apa102Strip.shift(1);
			// }

			// counter++;

			// if (counter > 1024){
			// 	counter = 0;
			// }
		// }


		ESP_LOGI(this->tag.c_str(), "Example end");
	}

	void powerUpFactoryReset(){

		ClearNvsButton* clearNvsButton = new ClearNvsButton();

		ESP_LOGI(this->tag.c_str(), "Press BOOT button now to factory reset.");

		vTaskDelay(2000 / portTICK_RATE_MS);

		ESP_LOGI(this->tag.c_str(), "Continuing.");

		delete clearNvsButton;
	}
};

extern "C" void app_main(void){
	try {
		static Main main;
	}
	catch(Notice& e) {
		ESP_LOGI("app_main", "%s", e.what());
	}
	catch(Warn& e) {
		ESP_LOGW("app_main", "%s", e.what());
	}
	catch(Error& e) {
		ESP_LOGE("app_main", "%s", e.what());
	}
	catch(const std::runtime_error& e){
		ESP_LOGE("app_main", "Runtime error: %s", e.what());
	}
	catch(const std::exception& e){
		ESP_LOGE("app_main", "Error occurred: %s", e.what());
	}
	catch(...){
		ESP_LOGE("app_main", "Unknown failure occurred. Possible memory corruption.");
	}
};