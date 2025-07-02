#pragma once
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class TemperatureMeterModule {
  public:
    TemperatureMeterModule(const char* serverUrl, const char* mac, int temperaturePin);
    void begin(unsigned long intervalMs);
    void handle();
  private:
    bool sendTemperature();
    bool checkTempClose();
    void sendCommand();
    const char*        _serverUrl;
    String             _mac;
    OneWire            _tempSensor;
    DallasTemperature  _sensor;
    unsigned long      _interval;
    unsigned long      _lastLecture;
    int                _temp;
};