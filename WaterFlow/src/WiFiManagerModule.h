#pragma once
#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class WiFiManagerModule {
  public:
    WiFiManagerModule(const char* serverUrl);
    void begin();                
    String getPairingToken();   
    String getMAC();             
    bool getIsInDatabase();
  private:
    const char* _serverUrl;
    String       _pairingToken;
    bool _isInDatabase;
};