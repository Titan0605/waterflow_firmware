#include "WiFiManagerModule.h"
#include <HTTPClient.h>

WiFiManagerModule::WiFiManagerModule(const char* serverUrl)
 : _serverUrl(serverUrl), _pairingToken("") {}

void WiFiManagerModule::begin() {
  Serial.println("Matching Wi-Fi...");

  // 1) Campo extra para el token
  WiFiManagerParameter custom_token("token", "Match code", "", 40);

  // 2) Configuramos WiFiManager
  WiFiManager wm;
  wm.addParameter(&custom_token);

  // 3) Inyectamos CSS en el <head> del portal
  wm.setCustomHeadElement(
    "<style>"
      "body { font-family: Arial, sans-serif; background: #f0f8ff; }"
      ".wm-input { margin-bottom: 8px; display: block; padding: 6px; width: 100%; }"
      "h1 { color: #007acc; font-size: 1.6em; text-align: center; }"
      "label { font-weight: bold; }"
      "button { background: #007acc; color: white; padding: 10px 20px; border: none; cursor: pointer; }"
      "button:hover { background: #005b99; }"
    "</style>"
  );

  // 4) Lanzamos portal cautivo
  if (!wm.autoConnect("WaterFlow_Setup")) {
    Serial.println("Wi-Fi error, rebooting...");
    wm.resetSettings();
    delay(3000);
    ESP.restart();
  }

  // 5) Obtenemos el token ingresado
  Serial.println("Conecting to WiFi.");
  _pairingToken = custom_token.getValue();
  Serial.printf("Token received: %s\n", _pairingToken.c_str());

  // 6) Enviamos al servidor Flask para validar y emparejar
  if(!this->getIsInDatabase()){
    HTTPClient http;
    String url = String(_serverUrl) + "/send-token";
    http.begin(url);
    http.addHeader("Content-Type","application/json");
    String body = "{\"waterflow_mac\":\"" + WiFi.macAddress() +
                  "\",\"token\":\"" + _pairingToken + "\"}";

    int code = http.POST(body);
    if (code == HTTP_CODE_CREATED) {
      String payload = http.getString();
      http.end();  // liberar antes del parseo

      // Parsear JSON de respuesta
      StaticJsonDocument<201> doc;
      auto err = deserializeJson(doc, payload);
      if (err) {
        Serial.println("Error parsing JSON, rebooting...");
        wm.resetSettings();
        delay(2000);
        ESP.restart();
      }
      Serial.printf("matched correctly (HTTP %d)\n", code);

    } else {
      Serial.printf("HTTP error %d, rebooting...\n", code);
      http.end();  // liberar antes de reiniciar
      wm.resetSettings();
      delay(2000);
      ESP.restart();
    }
  }
}

String WiFiManagerModule::getPairingToken() {
  return _pairingToken;
}

String WiFiManagerModule::getMAC() {
  return WiFi.macAddress();
}

bool WiFiManagerModule::getIsInDatabase(){
  String mac = this->getMAC();
  String url = String(_serverUrl) + "/in-database?waterflow_mac=" + mac.c_str();
  HTTPClient http_check;
  http_check.begin(url);
  int code = http_check.GET();
  if(code == HTTP_CODE_OK){
    String payload = http_check.getString();
    StaticJsonDocument<200> doc;
    DeserializationError err = deserializeJson(doc, payload);
    if(!err){
      return doc["is_in_database"];
    } else {
      Serial.println("Error parsing JSON");
      return false;
    }
  } else {
    Serial.printf("HTTP error: %d\n", code);
    return false;
  }
}