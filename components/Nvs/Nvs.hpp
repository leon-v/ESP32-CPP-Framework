#pragma once

#include "nvs_flash.h"
#include "esp_log.h"

#include "Exceptions.hpp"

#include <string>

using namespace std;

class Nvs{

	public:
	string tag = "Nvs";
	string name;
	nvs_handle handle;

	Nvs(){
		Nvs::initNvs();
	}

	Nvs(string name){

		Nvs::initNvs();

		this->name = name;

		
		ESP_ERROR_CHECK(nvs_open(this->name.c_str(), NVS_READWRITE, &this->handle));
	}

	string getString(string name) {

		size_t length = 0;

		ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_get_str(this->handle, name.c_str(), NULL, &length));

		if (length <= 0) {
			return "";
		}

		char* pCString = (char *) malloc((length + 1) * sizeof(char));

		if (!pCString) {
			throw Error("Failed to allocate memory for string");
			return "";
		}

		ESP_ERROR_CHECK(nvs_get_str(this->handle, name.c_str(), pCString, &length));

		string value = string(pCString);

		free(pCString);

		return value;
	}

	void setString(string name, string value) {
		ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_set_str(this->handle, name.c_str(), value.c_str()));
	}

	int getInt(string name){
		int64_t value = 0;
		ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_get_i64(this->handle, name.c_str(), &value));
		return value;
	}

	void setInt(string name, int64_t value){
		ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_set_i64(this->handle, name.c_str(), value));
	}

	static Nvs load(string name){
		Nvs nvs(name);
		return nvs;
	}

	void commit(){
		ESP_ERROR_CHECK(nvs_commit(this->handle));
	}
	
	static void initNvs(){

		static bool nvsInitalised = false;

		if (nvsInitalised) {
			return;
		}

		esp_err_t err = nvs_flash_init();

		if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
			// NVS partition was truncated and needs to be erased
			// Retry nvs_flash_init
			Nvs::reset();
			ESP_ERROR_CHECK(nvs_flash_init());
		}

		nvsInitalised = true;
	}

	static void reset(){
		ESP_ERROR_CHECK(nvs_flash_erase());
	}
};