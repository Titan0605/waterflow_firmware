#pragma once
#include <ArduinoJson.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class TemperatureMeterModule {
    public:
        TemperatureMeterModule(const char* serverUrl, const char* mac);
        void begin(int temperaturePin, unsigned long intervalMs);
        void handle();
        bool getState();
        bool sendTemperature();
        bool sendCommand(bool close);
        bool checkTempClose();
    private:
        const char* _serverUrl;
        String _mac;
        OneWire _tempSensor;
        DallasTemperature _sensor;
        unsigned long  _interval;
        unsigned long _lastLecture;
        int _temp;
};