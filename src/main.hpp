#pragma once

#include <Arduino.h>
#include <espnow.h>

#define NUM_HAT_LEDS 14

struct led {
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;
  uint8_t brightness = 0;

  void setWhite() {
    red = 255;
    green = 255;
    blue = 255;
  }
};

struct CommunicationData {
  bool isDoingAllowingEntryRoutine;
  bool isDoingDenyingEntryRoutine;
  unsigned long RoutineStartTime;
};

struct HatControlData {
  led leds[NUM_HAT_LEDS];
};

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
  uint32_t lastPressTime;
  uint32_t lastReleaseTime;
  bool lastReleaseHandled;
  bool lastPressHandled;
  // Returns how long the button was held last
  // will be invalid on the next button press
  uint32_t lastHeldTime() {
    if (lastPressTime < lastReleaseTime) {
      return lastReleaseTime - lastPressTime;
    }
    return 0;
  }
};

Buzzer::Melody_t denyTone{
    .nbNotes = 24,
    .duration = {80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80,
                 80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 80},
    .frequency = {
        E4_NOTE_FREQ, A4_NOTE_FREQ, G5_NOTE_FREQ, E4_NOTE_FREQ, A4_NOTE_FREQ,
        G5_NOTE_FREQ, E4_NOTE_FREQ, A4_NOTE_FREQ, G5_NOTE_FREQ, E4_NOTE_FREQ,
        A4_NOTE_FREQ, G5_NOTE_FREQ, E4_NOTE_FREQ, A4_NOTE_FREQ, G5_NOTE_FREQ,
        E4_NOTE_FREQ, A4_NOTE_FREQ, G5_NOTE_FREQ, E4_NOTE_FREQ, A4_NOTE_FREQ,
        G5_NOTE_FREQ, E4_NOTE_FREQ, A4_NOTE_FREQ, G5_NOTE_FREQ, E4_NOTE_FREQ,
        A4_NOTE_FREQ, G5_NOTE_FREQ, E4_NOTE_FREQ, A4_NOTE_FREQ, G5_NOTE_FREQ}};

Buzzer::Melody_t acceptTone{
    .nbNotes = 4,
    .duration = {50, 50, 100, 200},
    .frequency = {F5_NOTE_FREQ, G5_NOTE_FREQ, A5_NOTE_FREQ, B5_NOTE_FREQ}};

// Routine Handlers
// Update the current state of device based on ControlData
void updateSelf(HatControlData &aControlData);
// Update aControlData to make it do the routine for allow entry
void doAllowEntryRoutineUpdate(HatControlData &aControlData);
// Update aControlData to make it do the routine for deny entry
void doDenyEntryRoutineUpdate(HatControlData &aControlData);
// Maybe some Kind of breathing animation or something?
void doIdle(HatControlData &aControlData);