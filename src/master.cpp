#include <Arduino.h>
#include <Buzzer.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "main.hpp"

// Doorman
unsigned long lastTimeBellBoyRanErand = 0;
// 100ms between updates to slave
static constexpr auto timeBetweenErand = 1000;

HatControlData doorManData; // Controller of the 2 headsets
HatControlData bellBoyData; // Controlled by the 1 headset
// variables to keep track of the timing of recent interrupts

CommunicationData aDataToSendToSlave{.isDoingAllowingEntryRoutine = false,
                                     .isDoingDenyingEntryRoutine = false,
                                     .RoutineStartTime = 0};

uint8_t bellBoyAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

Button button1 = {D6, 0, false, 0, 0, false, false};
uint32_t numHandledPresses = 0;
Buzzer buzz1(D7);

Buzzer::Melody_t acceptTone{
    .nbNotes = 12,
    .duration = {80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80},
    .frequency = {E4_NOTE_FREQ, B4_NOTE_FREQ, E5_NOTE_FREQ, E4_NOTE_FREQ,
                  B4_NOTE_FREQ, E5_NOTE_FREQ, E4_NOTE_FREQ, B4_NOTE_FREQ,
                  E5_NOTE_FREQ, E4_NOTE_FREQ, B4_NOTE_FREQ, E5_NOTE_FREQ,
                  E4_NOTE_FREQ, B4_NOTE_FREQ, E5_NOTE_FREQ, E4_NOTE_FREQ,
                  B4_NOTE_FREQ, E5_NOTE_FREQ}};

void onDataSend(uint8 *mac_addr, uint8_t sentStatus);

// Send Update to slave controller to let it know we are starting or stopping
// routines.
void SendSlaveUpdate();
// Check if we need to start a routine
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

void boardLoop() {
  checkRoutineStart();

  if (aDataToSendToSlave.isDoingAllowingEntryRoutine) {
    doAllowEntryRoutineUpdate(doorManData);
  } else if (aDataToSendToSlave.isDoingDenyingEntryRoutine) {
    doDenyEntryRoutineUpdate(doorManData);
  } else {
    doIdle(doorManData);
  }
  updateSelf(doorManData);

  buzz1.step();
}

void onDataSend(uint8 *mac_addr, uint8_t sentStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sentStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}

void doAllowEntryRoutineUpdate(HatControlData &aControlData) {
  Serial.print("AllowEntry!!");
  buzz1.setMelody(&acceptTone);
  aDataToSendToSlave.isDoingAllowingEntryRoutine = false;
}

void doDenyEntryRoutineUpdate(HatControlData &aControlData) {
  Serial.println("DenyEntry!!");
  aDataToSendToSlave.isDoingDenyingEntryRoutine = false;
}

void doIdle(HatControlData &aControlData) {
  // auto current_time = millis();

  // for (int i = 0; i <)
}

void checkRoutineStart() {
  // Only Start Routines on Releases
  if (button1.pressed) {
    // Cancel routines on press so we have a way to quickly cancel
    aDataToSendToSlave.isDoingAllowingEntryRoutine = false;
    aDataToSendToSlave.isDoingDenyingEntryRoutine = false;
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

    aDataToSendToSlave.RoutineStartTime = millis();
    aDataToSendToSlave.isDoingDenyingEntryRoutine = true;
    SendSlaveUpdate();
    return;
  }
  // quick press does allow entry
  if (button1.lastHeldTime() > 50) {
    button1.lastReleaseHandled = true;

    aDataToSendToSlave.RoutineStartTime = millis();
    aDataToSendToSlave.isDoingAllowingEntryRoutine = true;
    SendSlaveUpdate();
  }
}

void SendSlaveUpdate() {
  if (auto err = esp_now_send(bellBoyAddress, (uint8_t *)&aDataToSendToSlave,
                              sizeof(aDataToSendToSlave));
      err != 0) {
    Serial.print("Error adding peer: ");
    Serial.println(err);
  }
}

void updateSelf(HatControlData &aControlData) {}