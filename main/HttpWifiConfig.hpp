#pragma once

#include "esp_log.h"

#include "Nvs.hpp"
#include "HttpServer.hpp"
#include "HttpServerUri.hpp"
#include "WifiClient.hpp"

#include <string>

using namespace std;

extern const char httpWifiConfigGetUriHtml[] asm("HttpWifiConfig_bin_html");

class HttpWifiConfig{

	public:
	HttpServer* pHttpServer;
	Wifi* pWifi;
	Nvs* pWifiConfig;
	vector <WifiClientScannedAp> pWifiClientScannedAps;

	class HttpWifiConfigGetUri : public HttpServerUri{

		public:
		HttpWifiConfig* pHttpWifiConfig;

		HttpWifiConfigGetUri(HttpWifiConfig* pHttpWifiConfig) : 
			HttpServerUri("/wifi-config", HTTP_GET)
		{
			this->pHttpWifiConfig = pHttpWifiConfig;
		}

		void handler(httpd_req_t* pRequest) {
			this->respond(pRequest, pHttpWifiConfig->getPageHtml(""));
		}
	};
	HttpWifiConfigGetUri httpWifiConfigGetUri;

	class HttpWifiConfigPostUri : public HttpServerUri{
		
		public:
		HttpWifiConfig* pHttpWifiConfig;

		HttpWifiConfigPostUri(HttpWifiConfig* pHttpWifiConfig) :
			HttpServerUri("/wifi-config", HTTP_POST)
		{
			this->pHttpWifiConfig = pHttpWifiConfig;
		}

		void handler(httpd_req_t* pRequest) {

			string postBody = this->getPost(pRequest);

			string inputSsid = this->getQueryValue(postBody, "inputSsid");
			this->pHttpWifiConfig->pWifiConfig->setString("ssid", inputSsid);

			string password = this->getQueryValue(postBody, "password");
			this->pHttpWifiConfig->pWifiConfig->setString("password", password);

			string authModeKey = this->getQueryValue(postBody, "authMode");
			wifi_auth_mode_t authMode = Wifi::getAuthModeFromKey(authModeKey);
			this->pHttpWifiConfig->pWifiConfig->setInt("authMode", (int) authMode);
			
			
			this->pHttpWifiConfig->pWifiConfig->commit();

			this->respond(pRequest, pHttpWifiConfig->getPageHtml("Saved"));
		}
	};
	HttpWifiConfigPostUri httpWifiConfigPostUri;

	HttpWifiConfig(HttpServer* pHttpServer, Wifi* pWifi, Nvs* pWifiConfig) :
		httpWifiConfigGetUri(this),
		httpWifiConfigPostUri(this)
		{

		this->pHttpServer = pHttpServer;
		this->pWifiConfig = pWifiConfig;
		this->pWifi = pWifi;

		this->pHttpServer->registerUri(&this->httpWifiConfigGetUri);
		this->pHttpServer->registerUri(&this->httpWifiConfigPostUri);

		this->pWifi->client.start();

		this->setScannedAps(this->pWifi->client.getScan());

		this->pWifi->ap.start();
	}

	void setScannedAps(vector <WifiClientScannedAp> pWifiClientScannedAps){
		this->pWifiClientScannedAps = pWifiClientScannedAps;
	}

	string getPageHtml(string message){

		string pageHtml = string(httpWifiConfigGetUriHtml);

		HttpServerUri::htmlReplace(pageHtml, "[MESSAGE]", message);

		string currentSsid = this->pWifiConfig->getString("ssid");
		HttpServerUri::htmlReplace(pageHtml, "[SSID]", HttpServerUri::htmlEncode(currentSsid));

		string currentPassword = this->pWifiConfig->getString("password");
		HttpServerUri::htmlReplace(pageHtml, "[PASSWORD]", HttpServerUri::htmlEncode(currentPassword));

		
		wifi_auth_mode_t authMode = (wifi_auth_mode_t) this->pWifiConfig->getInt("authMode");
		string authModeKey = Wifi::getAuthModeKey(authMode);

		string authModeSelected = "[";
		authModeSelected.append(authModeKey);
		authModeSelected.append("_SELECTED]");
		HttpServerUri::htmlReplace(pageHtml, authModeSelected, "selected");


		string ssidOptions;
		for(WifiClientScannedAp wifiClientScannedAp : this->pWifiClientScannedAps) {
			
			ssidOptions.append("<option ");

			ssidOptions.append("value=\"");
			ssidOptions.append(HttpServerUri::htmlEncode(wifiClientScannedAp.ssid));
			ssidOptions.append("\" ");

			ssidOptions.append("auth-mode=\"");
			ssidOptions.append(HttpServerUri::htmlEncode(Wifi::getAuthModeKey(wifiClientScannedAp.authMode)));
			ssidOptions.append("\" ");

			if (wifiClientScannedAp.ssid.compare(currentSsid) == 0) {
				ssidOptions.append(" selected ");
			}

			ssidOptions.append(">");

			ssidOptions.append(HttpServerUri::htmlEncode(wifiClientScannedAp.ssid));
			ssidOptions.append("</option>");
		}

		HttpServerUri::htmlReplace(pageHtml, "[SSID_OPTIONS]", ssidOptions);

		return pageHtml;
	}

};