#include "PollingServoModule.h"
#include <WiFi.h>
#include <HTTPClient.h>

PollingServoModule::PollingServoModule(const char* serverUrl, const char* mac)
 : _serverUrl(serverUrl), _mac(mac), _interval(3000), _lastPoll(0), _lastState(false) {}

void PollingServoModule::begin(int servoPin, unsigned long intervalMs) {
  /*
    It puts the pin of the servo and gets the state of the waterflow (in case the waterflow was disconnected and re-connected)
    Args:
    servoPin (int): The pin where the signal is going to be sent
    intervalMs (unsigned long): How many time is going to wait until another time (the time is for requesting if the wateflow state is open or closed)
  */
  _servoPin = servoPin;
  pinMode(_servoPin, OUTPUT);
  _interval = intervalMs;
  if (this->getState()) {
      digitalWrite(_servoPin, HIGH);
      Serial.println("Valv activated");
    } else {
      digitalWrite(_servoPin, LOW);
      Serial.println("Valc closed");
    }
}

void PollingServoModule::handle() {
    /*
    This function is for checking every 5 seconds (or other time stablished) the state of the waterflow requesting the server for the information
  */
  unsigned long now = millis();
  if (now - _lastPoll < _interval) return;
  _lastPoll = now;

  if (WiFi.status() != WL_CONNECTED) return;

  bool currentState = this->getState();
  if (currentState != _lastState) {
    _lastState = currentState;
    if (currentState) {
      digitalWrite(_servoPin, HIGH);
      Serial.println("Valv activated");
    } else {
      digitalWrite(_servoPin, LOW);
      Serial.println("Valc closed");
    }
  } 
}

bool PollingServoModule::getState(){
    /*
    This function is for getting the state from the server
    Returns:
    boolean value: If the state of the waterflow is open then returns true, if not returns false
  */
  String url = String(_serverUrl) + "/waterflow-state?waterflow_mac=" + _mac;
  HTTPClient http;
  http.begin(url);
  int code = http.GET();
  if (code == HTTP_CODE_OK) {
    String payload = http.getString();
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