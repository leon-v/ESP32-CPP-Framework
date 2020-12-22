#pragma once

#include "esp_http_server.h"
#include "esp_log.h"
#include "sys/param.h"

#include <string>

using namespace std;

class HttpServerUri{

	public:

	string path;
	string tag = "HttpServerUri";

	httpd_uri_t uri;

	HttpServerUri(){
		this->uri.method = HTTP_GET;

		this->init();
	}

	HttpServerUri(string path){ 
		this->path = path;
		this->uri.method = HTTP_GET;

		this->init();
	}

	HttpServerUri(string path, httpd_method_t method){
		this->path = path;
		this->uri.method = method;

		this->init();
	}

	void init(){

		this->tag.append(": ");
		this->tag.append(path);

		this->uri.uri = this->path.c_str();
		this->uri.handler = HttpServerUri::staticHandler;
		this->uri.user_ctx = this;
	}
	
	virtual void handler(httpd_req_t* pRequest) {
		this->respond(pRequest, "No responder set");
	}

	void respond(httpd_req_t* pRequest, string html) {
		httpd_resp_send(pRequest, html.c_str(), html.length());
	}

	static esp_err_t staticHandler(httpd_req_t* pRequest) {
		
		HttpServerUri* httpServerUri = (HttpServerUri*) pRequest->user_ctx;

		httpServerUri->handler(pRequest);

		return ESP_OK;
	}

	static string getPost(httpd_req_t* pRequest){

		string result;
		int remaining = pRequest->content_len;

		while (remaining > 0) {

			char tempBuffer[32] = {0};

			int toGet = MIN(remaining, sizeof(tempBuffer) - 1);

			int revieveResult = httpd_req_recv(pRequest, tempBuffer, toGet);

			if (revieveResult <= 0) {

				if (revieveResult == HTTPD_SOCK_ERR_TIMEOUT) {
					/* Retry receiving if timeout occurred */
					continue;
				}

				return "";
			}

			tempBuffer[revieveResult] = '\0';
			remaining -= revieveResult;

			result.append(string(tempBuffer));
		}

		return result;
	}

	static string htmlEncode(string data) {

		string result;

		result.reserve(data.size());

		for(size_t pos = 0; pos != data.size(); ++pos) {
			switch(data[pos]) {
				case '&':  result.append("&amp;");       break;
				case '\"': result.append("&quot;");      break;
				case '\'': result.append("&apos;");      break;
				case '<':  result.append("&lt;");        break;
				case '>':  result.append("&gt;");        break;
				default:   result.append(&data[pos], 1); break;
			}
		}

		return result;
	}

	static bool htmlReplace(string& str, const string& from, const string& to) {

		size_t start_pos = str.find(from);

		if(start_pos == string::npos) {
			return false;
		}
			
		str.replace(start_pos, from.length(), to);

		return true;
	}

	static string getQueryValue(string query, string key){
		
		size_t itemStart = 0;
		size_t itemEnd = itemStart;
		size_t seperatorPos = itemStart;

		while (itemStart < query.length()) {

			itemEnd = query.find("&", itemStart);

			if (itemEnd == string::npos) {
				itemEnd = query.length() + 1;
			}

			seperatorPos = query.find("=", itemStart);

			if (seperatorPos == string::npos) {
				ESP_LOGE("getQueryValue", "Unable to find '=' in '%s'", query.c_str());
				itemStart = itemEnd + 1;
				continue;
			}

			string itemKey = query.substr(itemStart, seperatorPos - itemStart);

			itemKey = HttpServerUri::urlDecode(itemKey);

			if (itemKey.compare(key) != 0) {
				itemStart = itemEnd + 1;
				continue;
			}

			string itemValue = query.substr(seperatorPos + 1, itemEnd - seperatorPos - 1);

			return HttpServerUri::urlDecode(itemValue);
		}

		return "";
	}

	static string urlDecode(string &value) {

		string decoded;
		char character;
		int index, characterCode;

		for (index = 0; index < value.length(); index++) {

			if (int(value[index]) == 37) {

				sscanf(value.substr(index + 1, 2).c_str(), "%x", &characterCode);

				character = static_cast <char> (characterCode);

				decoded += character;
				index += 2;
			}
			else {
				decoded += value[index];
			}
		}

		return decoded;
	}
};