#pragma once
#include <ArduinoJson.h>

class TemperatureMeterModule {
    public:
        TemperatureMeterModule(const char* serverUrl, const char* mac);
        void begin(int temperaturePin, unsigned long intervalMs);
        void handle();
        bool getState();
    private:
        const char* _serverUrl;
        String _mac;
        
};