#include <Arduino.h>
#include <Buzzer.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "main.hpp"
// Manage and send communication data via the CheckRoutineStartFunction
CommunicationData communicationData = {.isDoingAllowingEntryRoutine = false,
                                       .isDoingDenyingEntryRoutine = false,
                                       .RoutineStartTime = 0};

uint8_t bellBoyAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

Button button1 = {D6, 0, false, 0, 0, false, false};

void onDataSend(uint8 *mac_addr, uint8_t sentStatus);

// Send Update to slave controller to let it know we are starting or stopping
// routines.
void SendSlaveUpdate();
// Check if we need to start a routine based on button presses
void checkRoutineStart();

void IRAM_ATTR button_isr() {
  auto current_time = millis();
  if (digitalRead(button1.PIN) == LOW &&
      current_time - button1.lastPressTime > 100) {
    button1.numberKeyPresses++;
    button1.pressed = true;
    button1.lastPressHandled = false;
    button1.lastPressTime = current_time;
  } else if (digitalRead(button1.PIN) == HIGH && button1.pressed) {
    button1.pressed = false;
    button1.lastReleaseHandled = false;
    button1.lastReleaseTime = millis();
  }
}

void boardSetup() {
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, button_isr, CHANGE);

  esp_now_register_send_cb(onDataSend);
  int err = esp_now_add_peer(bellBoyAddress, ESP_NOW_ROLE_COMBO, 1, nullptr, 0);

  if (0 == err) {
    Serial.println("Added peer successfully");
  } else {
    Serial.print("Error adding peer: ");
    Serial.println(err);
  }
}

void boardLoop() { checkRoutineStart(); }

void onDataSend(uint8 *mac_addr, uint8_t sentStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sentStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}

void checkRoutineStart() {
  // Only Start Routines on Releases
  if (button1.pressed) {
    // Cancel routines on press so we have a way to quickly cancel
    communicationData.isDoingAllowingEntryRoutine = false;
    communicationData.isDoingDenyingEntryRoutine = false;
    // Send update once right on press.
    if (!button1.lastPressHandled) {
      SendSlaveUpdate();
      button1.lastPressHandled = true;
    }
    return;
  }
  if (button1.lastReleaseHandled) {
    return;
  }

  // One Second or longer press does no routine this will act a cancel
  if (button1.lastHeldTime() > 1000) {
    button1.lastReleaseHandled = true;

    return;
  }
  // Half second press does deny
  if (button1.lastHeldTime() > 500) {
    button1.lastReleaseHandled = true;

    communicationData.RoutineStartTime = millis();
    communicationData.isDoingDenyingEntryRoutine = true;
    SendSlaveUpdate();
    return;
  }
  // quick press does allow entry
  if (button1.lastHeldTime() > 50) {
    button1.lastReleaseHandled = true;

    communicationData.RoutineStartTime = millis();
    communicationData.isDoingAllowingEntryRoutine = true;
    SendSlaveUpdate();
  }
}

void SendSlaveUpdate() {
  if (auto err = esp_now_send(bellBoyAddress, (uint8_t *)&communicationData,
                              sizeof(communicationData));
      err != 0) {
    Serial.print("Error adding peer: ");
    Serial.println(err);
  }
}
