#include "routines.hpp"
#include <Arduino.h>
#include <Buzzer.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// Manage and send communication data via the CheckRoutineStartFunction
CommunicationData communicationData = {

    .isRequestingAllowingEntryRoutine = false,
    .isRequestingDenyingEntryRoutine = false,
    .isRequestingMarioRoutine = false,
    .isRequestingJeopardyRoutine = false,
    .isLightsOnInIdle = true};

uint8_t david[] = {0x24, 0xD7, 0xEB, 0xCA, 0x81, 0x6C};
uint8_t josh[] = {0x24, 0xD7, 0xEB, 0xCA, 0x6E, 0xAA};

uint8_t john[] = {0x24, 0xD7, 0xEB, 0xCA};
uint8_t katie[] = {0x24, 0xD7, 0xEB, 0xCA};

uint8_t matt[] = {0x24, 0xD7, 0xEB, 0xCA};
uint8_t zach[] = {0x24, 0xD7, 0xEB, 0xCA};

uint8_t nathan[] = {0x24, 0xD7, 0xEB, 0xCA};
uint8_t dan[] = {0x24, 0xD7, 0xEB, 0xCA};

uint8_t sam[] = {0x24, 0xD7, 0xEB, 0xCA};
uint8_t kristin[] = {0x24, 0xD7, 0xEB, 0xCA};

uint8_t *buddyAddress = josh;

Button button1 = {D1, 0, false, 0, 0, false, false};

void onDataSend(uint8 *mac_addr, uint8_t sentStatus);
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len);

// Send Update to slave controller to let it know we are starting or stopping
// routines.
void SendBuddyUpdate();
// Check if we need to start a routine based on button presses
void checkRoutineStart();

void IRAM_ATTR button_isr() {
  auto current_time = millis();
  if (digitalRead(button1.PIN) == LOW &&
      current_time - button1.lastPressTime > 50) {
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
  // Serial.print("MASTER ADDRESS:");
  // Serial.println(WiFi.macAddress());

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, button_isr, CHANGE);

  esp_now_register_send_cb(onDataSend);
  esp_now_register_recv_cb(OnDataRecv);
  int err = esp_now_add_peer(buddyAddress, ESP_NOW_ROLE_COMBO, 1, nullptr, 0);

  if (0 == err) {
    Serial.println("Added peer successfully");
  } else {
    Serial.print("Error adding peer: ");
    Serial.println(err);
  }
}

void boardLoop() { checkRoutineStart(); }

void onDataSend(uint8 *mac_addr, uint8_t sentStatus) {
  // Serial.print("Last Packet Send Status: ");
  if (sentStatus == 0) {
    // Serial.println("Delivery success");
  } else {
    // Serial.println("Delivery fail");
  }
}

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&communicationData, incomingData, sizeof(CommunicationData));
  Serial.println("Got Data");
}

void checkRoutineStart() {
  if (button1.pressed) {
    // Cancel routines on press so we have a way to quickly cancel
    // Send update once right on press.
    if (!button1.lastPressHandled) {
      cancelRoutines();
      Serial.println("Canceled Routines");
      SendBuddyUpdate();
      button1.lastPressHandled = true;
    }
    return;
  }

  // Only Start Routines on Releases
  if (button1.lastReleaseHandled) {
    return;
  }

  // One Second or longer press does no routine this will act a cancel
  if (button1.lastHeldTime() > 3000) {
    button1.lastReleaseHandled = true;

    return;
  }

  if (button1.lastHeldTime() > 2000) {
    button1.lastReleaseHandled = true;
    communicationData.isRequestingMarioRoutine = true;
    SendBuddyUpdate();
    return;
  }

  // Half second press does deny
  if (button1.lastHeldTime() > 500) {
    button1.lastReleaseHandled = true;
    communicationData.isRequestingDenyingEntryRoutine = true;
    SendBuddyUpdate();
    return;
  }
  // quick press does allow entry
  if (button1.lastHeldTime() > 30) {
    button1.lastReleaseHandled = true;
    communicationData.isRequestingAllowingEntryRoutine = true;
    SendBuddyUpdate();
  }
}

void SendBuddyUpdate() {
  if (auto err = esp_now_send(buddyAddress, (uint8_t *)&communicationData,
                              sizeof(CommunicationData));
      err != 0) {
    Serial.print("Error adding peer: ");
    Serial.println(err);
  }
}
