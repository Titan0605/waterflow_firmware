#include "TemperatureMeterModule.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

TemperatureMeterModule::TemperatureMeterModule(const char* serverUrl, const char* mac, int temperaturePin)
  : _serverUrl(serverUrl),
    _mac(mac),
    _tempSensor(temperaturePin),
    _sensor(&_tempSensor),
    _interval(60000),
    _lastLecture(0),
    _temp(0)
{}

void TemperatureMeterModule::begin(unsigned long intervalMs){
  _sensor.begin(); 
  _interval = intervalMs;
}

void TemperatureMeterModule::handle(){
  unsigned long now = millis();
  if (now - _lastLecture < _interval) return;
  _lastLecture = now;

  if (WiFi.status() != WL_CONNECTED) return;

  if (sendTemperature()){
    Serial.println("Temperature sent successfully");
  } else {
    Serial.println("Failed to send temperature");
  }
}

bool TemperatureMeterModule::sendTemperature(){
  HTTPClient http;
  String url = String(_serverUrl) + "/send-temperature";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  _sensor.requestTemperatures();
  int temp = _sensor.getTempCByIndex(0);

  StaticJsonDocument<200> doc;
  doc["mac_address"] = _mac;
  doc["temp"]        = temp;
  String jsonData;
  serializeJson(doc, jsonData);

  int code = http.POST(jsonData);
  http.end();  

  if (code == HTTP_CODE_OK){
    _temp = temp;
    return checkTempClose();
  } 
  Serial.printf("HTTP POST error: %d\n", code);
  return false;
}

bool TemperatureMeterModule::checkTempClose(){
  HTTPClient http;
  String url = String(_serverUrl) + "/get-temperature-waterflow?mac_address=" + _mac;
  http.begin(url);
  int code = http.GET();

  if (code == HTTP_CODE_OK){
    String payload = http.getString();
    StaticJsonDocument<200> doc;
    DeserializationError err = deserializeJson(doc, payload);
    http.end();

    if (!err){
      bool autoClose    = doc["autoClose"];
      int  autoCloseTemp = doc["autoCloseTemp"];
      if (autoClose && (_temp < autoCloseTemp)){
        Serial.println("Autoclose triggered");
        sendCommand();
        return true;
      }
      Serial.println("Autoclose not needed");
      return true;
    }
    Serial.println("JSON parse error");
    return false;
  }
  http.end();
  Serial.printf("HTTP GET error: %d\n", code);
  return false;
}

void TemperatureMeterModule::sendCommand(){
  HTTPClient http;
  String url = String(_serverUrl) + "/send-command";
  http.begin(url);
  http.addHeader("Content-Type","application/json");

  StaticJsonDocument<200> doc;
  doc["waterflow_mac"] = _mac;
  doc["activate"]      = true;
  String jsonData;
  serializeJson(doc, jsonData);

  int code = http.POST(jsonData);
  http.end();

  if (code == HTTP_CODE_OK){
    Serial.println("Command POST successful");
  } else {
    Serial.printf("Command POST failed: %d\n", code);
  }
}