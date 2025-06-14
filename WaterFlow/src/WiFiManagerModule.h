#pragma once
#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class WiFiManagerModule {
  public:
    WiFiManagerModule(const char* serverUrl);
    void begin();                // Empareja y conecta Wi-Fi
    String getPairingToken();    // Recupera el token que ingresó el usuario
    String getMAC();             // Devuelve la MAC
    bool getIsInDatabase();
  private:
    const char* _serverUrl;
    String       _pairingToken;
    bool _isInDatabase;
};