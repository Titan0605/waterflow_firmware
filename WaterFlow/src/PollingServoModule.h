#pragma once
#include <ArduinoJson.h>

class PollingServoModule {
  public:
    PollingServoModule(const char* serverUrl, const char* mac);
    void begin(int servoPin, unsigned long intervalMs);
    void handle();
    bool getState();
  private:
    const char*    _serverUrl;
    String         _mac;
    int            _servoPin;
    unsigned long  _interval;
    unsigned long  _lastPoll;
    bool           _lastState = false; 
};
