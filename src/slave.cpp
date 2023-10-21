#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "main.hpp"

// Slave receives the data from master and then runs the same logic.
CommunicationData communicationData = {.isDoingAllowingEntryRoutine = false,
                                       .isDoingDenyingEntryRoutine = false,
                                       .RoutineStartTime = 0};

uint8_t doorManAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&communicationData, incomingData, sizeof(communicationData));
  Serial.println("Got Data");
}

void boardSetup() {
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