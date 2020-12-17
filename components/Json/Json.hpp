#pragma once

#include "esp_log.h"
#include <cJSON.h>

#include <string>
#include <iterator>

using namespace std;

class Json {

	public:
	string tag = "Json";
	cJSON *pCJson;
	
	Json(cJSON* json){
		this->pCJson = json;
	}

	Json(string input){

		this->pCJson = cJSON_Parse(input.c_str());
		
		ESP_LOGI(this->tag.c_str(), "Initalised");
	}

	~Json(){
		cJSON_Delete(this->pCJson);
	}

	bool isRaw(){
		return (bool) cJSON_IsRaw(this->pCJson);
	}

	bool isReference(){
		return (bool) this->pCJson->type & cJSON_IsReference;
	}
	
	bool isObject(){
		return (bool) cJSON_IsObject(this->pCJson);
	}

	bool isArray(){
		return (bool) cJSON_IsArray(this->pCJson);
	}

	bool isString(){
		return (bool) cJSON_IsString(this->pCJson);
	}

	bool isStringConst(){

		if (this->isString()) {
			return (bool) this->pCJson->type & cJSON_StringIsConst;
		}
		
		throw "Is string const called on non-string";
	}

	bool isInvalid(){
		return (bool) cJSON_IsInvalid(this->pCJson);
	}

	bool isBool(){
		return (bool) cJSON_IsBool(this->pCJson);
	}

	bool isFalse(){

		if (this->isBool()) {
			return (bool) cJSON_IsFalse(this->pCJson);
		}
		
		throw "Is falsed called on non-boolean";
	}

	bool isTrue(){

		if (this->isBool()) {
			return (bool) cJSON_IsTrue(this->pCJson);
		}
		
		throw "Is true called on no-boolean";
	}

	bool isNull(){
		return (bool) cJSON_IsNull(this->pCJson);
	}

	bool isNumber(){
		return (bool) cJSON_IsNumber(this->pCJson);
	}

	Json getItem(string name){

		cJSON* pCJson = cJSON_GetObjectItemCaseSensitive(this->pCJson, name.c_str());	

		Json json(pCJson);
		
		return json;
	}

	static Json createArray(){

		cJSON* pCJson = cJSON_CreateArray();

		Json json(pCJson);
		
		return json;
	}

	static Json createString(string value){
		Json json(cJSON_CreateString(value.c_str()));
		return json;
	}

	static Json CreateNumber(double value){
		Json json(cJSON_CreateNumber(value));
		return json;
	}

	static Json createNull(){
		Json json(cJSON_CreateNull());
		return json;
	}

	static Json createBool(bool value){
		Json json(cJSON_CreateBool( (cJSON_bool) value));
		return json;
	}

	static Json createTrue(){
		Json json(cJSON_CreateTrue());
		return json;
	}

	static Json createFalse(){
		Json json(cJSON_CreateFalse());
		return json;
	}

	void add(string name, Json value){
		this->addItem(name, value);	
	}

	void add(string value){
		this->addString(value);
	}

	void add(string name, string value){
		this->addString(name, value);
	}

	void addItem(Json value){

		if (this->isArray()){
			cJSON_AddItemToArray(this->pCJson, value.pCJson);
			return;
		}

		throw "Add item called on non-array";
	}

	void addItem(string name, Json value){

		if (this->isObject()){
			cJSON_AddItemToObject(this->pCJson, name.c_str(), value.pCJson);
			return;
		}

		throw "Add item called on non-object";
	}

	void addString(string value){

		if (this->isArray()){
			Json json = Json::createString(value);
			cJSON_AddItemToArray(this->pCJson, json.pCJson);
			return;
		}

		throw "Add string called on non-array";
	}

	void addString(string name, string value){

		if (this->isObject()){
			cJSON_AddStringToObject(this->pCJson, name.c_str(), value.c_str());
			return;
		}

		throw "Add string called on non-object";
	}

	// void setValue(string value){

	// 	if (this->isString()) {
	// 		cJSON_SetValuestring()
	// 	}

	// 	throw "Set value string called on non-string";
	// }

	string toString() {
		char *jsonChars = cJSON_Print(this->pCJson);
		string jsonString(jsonChars);
		return jsonString;
	}
};