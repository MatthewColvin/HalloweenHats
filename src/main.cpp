#include "routines.hpp"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

extern void boardSetup();
extern void boardLoop();

long unsigned int lastHeartBeatToggle = 0;
constexpr auto HeartBeatHalfCycleLength = 500;
bool builtInOn = false;
void doHeartBeat() {
  if (lastHeartBeatToggle + HeartBeatHalfCycleLength < millis()) {
    digitalWrite(LED_BUILTIN, builtInOn ? LOW : HIGH);
    builtInOn = !builtInOn;
    lastHeartBeatToggle = millis();
  }
}

void setup() {
  // Common
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  pinMode(LED_BUILTIN, OUTPUT);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  boardSetup();
  HandleRoutineSetup();
}

void loop() {
  boardLoop();
  HandleRoutines();
  doHeartBeat();
}