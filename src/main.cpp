#include "routines.hpp"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

extern void boardSetup();
extern void boardLoop();

void setup() {
  // Common
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  boardSetup();
}

void loop() {
  boardLoop();
  HandleRoutines();
}