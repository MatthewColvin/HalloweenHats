#include "routines.hpp"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

/////////////////////////Depreciated////////////////////////////////////

// Slave receives the data from master and then runs the same logic.
CommunicationData communicationData = {
    .isRequestingAllowingEntryRoutine = false,
    .isRequestingDenyingEntryRoutine = false};

// uint8_t doorManAddress[] = {0x24, 0xD7, 0xEB, 0xCA, 0x6E, 0xAA};
uint8_t doorManAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&communicationData, incomingData, sizeof(CommunicationData));
  Serial.println("Got Data");
}

void boardSetup() {
  Serial.print("Slave Address:");
  Serial.println(WiFi.macAddress());

  pinMode(LED_BUILTIN, OUTPUT);
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  if (auto err =
          esp_now_add_peer(doorManAddress, ESP_NOW_ROLE_COMBO, 1, nullptr, 0);
      err != 0) {
    Serial.print("Error adding peer: ");
    Serial.println(err);
  }

  if (auto err = esp_now_register_recv_cb(OnDataRecv); err != 0) {
    Serial.print("Error setting callback: ");
    Serial.println(err);
  }
}

void boardLoop() {}