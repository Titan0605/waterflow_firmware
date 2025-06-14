#include <Arduino.h>
#include <WiFi.h>
#include "WiFiManagerModule.h"
#include "PollingServoModule.h"

const char* SERVER_URL = "https://vw6jc4s1-5000.usw3.devtunnels.ms";
const int   SERVO_PIN  = 23;
const int   POLL_MS    = 5000;

WiFiManagerModule wifiMod(SERVER_URL);
PollingServoModule pollMod(SERVER_URL, "");

void setup() {
  Serial.begin(115200);

  // 1) Emparejamiento y registro
  wifiMod.begin();
  
  // 2) Configura módulo de polling con la MAC recién obtenida
  String mac = wifiMod.getMAC();
  pollMod = PollingServoModule(SERVER_URL, mac.c_str());
  pollMod.begin(SERVO_PIN, POLL_MS);
}

void loop() {
  pollMod.handle();
}
