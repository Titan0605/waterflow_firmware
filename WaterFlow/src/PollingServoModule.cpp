#include "PollingServoModule.h"
#include <WiFi.h>
#include <HTTPClient.h>

PollingServoModule::PollingServoModule(const char* serverUrl, const char* mac)
 : _serverUrl(serverUrl), _mac(mac), _interval(5000), _lastPoll(0), _lastState(false) {}

void PollingServoModule::begin(int servoPin, unsigned long intervalMs) {
  _servo.attach(servoPin);
  _interval = intervalMs;
  if (this->getState()) {
      _servo.write(90);
      Serial.println("Valv activated");
    } else {
      _servo.write(0);
      Serial.println("Valc closed");
    }
}

void PollingServoModule::handle() {
  unsigned long now = millis();
  if (now - _lastPoll < _interval) return;
  _lastPoll = now;

  if (WiFi.status() != WL_CONNECTED) return;

  // Construir la URL con el parámetro correcto
  bool currentState = this->getState();
  // Solo actuamos si cambió respecto a la última vez
  if (currentState != _lastState) {
    _lastState = currentState;
    if (currentState) {
      _servo.write(90);
      Serial.println("Valv activated");
    } else {
      _servo.write(0);
      Serial.println("Valc closed");
    }
  } 
}

bool PollingServoModule::getState(){
  String url = String(_serverUrl) + "/waterflow-state?waterflow_mac=" + _mac;
  HTTPClient http;
  http.begin(url);
  int code = http.GET();
  if (code == HTTP_CODE_OK) {
    // Leer payload
    String payload = http.getString();
    // JSON parsing
    StaticJsonDocument<200> doc;
    DeserializationError err = deserializeJson(doc, payload);
    if (!err) {
      return doc["activate"];
    } else {
      Serial.println("Error parsing JSON");
      return false;
    }
  } else {
    Serial.printf("HTTP error: %d\n", code);
    return false;
  }
}