#include <Arduino.h>
#include <WiFi.h>
#include "WiFiManagerModule.h"
#include "PollingServoModule.h"
#include "TemperatureMeterModule.h"

const char* SERVER_URL = "https://cleanlyfe-production.up.railway.app/waterflow";
const int   SERVO_PIN  = 23;
const int   POLL_MS    = 3000;
const int   TEMP_PIN     = 32;       
const int   TEMP_INTERVAL = 30000;

WiFiManagerModule wifiMod(SERVER_URL);
PollingServoModule* pollMod = nullptr;
TemperatureMeterModule* tempMod = nullptr;

void setup() {
  Serial.begin(115200);

  wifiMod.begin();
  
  String mac = wifiMod.getMAC();
  
  pollMod = new PollingServoModule(SERVER_URL, mac.c_str());
  pollMod->begin(SERVO_PIN, POLL_MS);
  
  tempMod = new TemperatureMeterModule(SERVER_URL, mac.c_str(), TEMP_PIN);
  tempMod->begin(TEMP_INTERVAL);
  
  Serial.println("Setup completed with MAC: " + mac);
}

void loop() {
  if (pollMod) pollMod->handle();
  if (tempMod) tempMod->handle();
}