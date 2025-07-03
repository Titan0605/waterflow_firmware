#include <Arduino.h>
#include <WiFi.h>
#include "WiFiManagerModule.h"
#include "PollingServoModule.h"
#include "TemperatureMeterModule.h"

const char* SERVER_URL = "https://00ts5pkz-3000.usw3.devtunnels.ms/waterflow";
const int   SERVO_PIN  = 23;
const int   POLL_MS    = 5000;
const int   TEMP_PIN     = 32;       
const int   TEMP_INTERVAL = 60000;

WiFiManagerModule wifiMod(SERVER_URL);
PollingServoModule pollMod(SERVER_URL, "");
TemperatureMeterModule tempMod(SERVER_URL, "", TEMP_PIN);

void setup() {
  Serial.begin(115200);

  wifiMod.begin();
  
  String mac = wifiMod.getMAC();
  pollMod = PollingServoModule(SERVER_URL, mac.c_str());
  pollMod.begin(SERVO_PIN, POLL_MS);
  tempMod = TemperatureMeterModule(SERVER_URL, mac.c_str(), TEMP_PIN);
  tempMod.begin(TEMP_INTERVAL);
}

void loop() {
  pollMod.handle();
  tempMod.handle();
}
