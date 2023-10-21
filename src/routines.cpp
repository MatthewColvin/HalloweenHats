#include "routines.hpp"
#include "main.hpp" // Shared Communication Data
#include <Buzzer.h>

Buzzer buzz1(D7);
HatControlData controlData;

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

void doAllowEntryRoutineUpdate(HatControlData &aControlData) {
  Serial.print("AllowEntry!!");
  buzz1.setMelody(&acceptTone);
  communicationData.isDoingAllowingEntryRoutine = false;
}

void doDenyEntryRoutineUpdate(HatControlData &aControlData) {
  Serial.println("DenyEntry!!");
  buzz1.setMelody(&denyTone);
  communicationData.isDoingDenyingEntryRoutine = false;
}

constexpr auto halfBreathTime = 1000;
uint32_t numIdleCycles = 0;
void doIdle(HatControlData &aControlData) {
  auto currentTime = millis();
  numIdleCycles = currentTime / halfBreathTime;
  auto currentPortion = currentTime % halfBreathTime;

  bool isBreathOut = numIdleCycles % 2 == 0;

  for (int i = 0; i < NUM_HAT_LEDS; i++) {
    aControlData.leds[i].setWhite();
    auto value = 255 - map(currentPortion, 0, halfBreathTime, 0, 255);
    auto brightness = isBreathOut ? 255 - value : value;
    aControlData.leds[i].brightness = brightness;
  }
}

void updateSelf(HatControlData &aControlData) {}

void HandleRoutines() {
  if (communicationData.isDoingAllowingEntryRoutine) {
    doAllowEntryRoutineUpdate(controlData);
  } else if (communicationData.isDoingDenyingEntryRoutine) {
    doDenyEntryRoutineUpdate(controlData);
  } else {
    doIdle(controlData);
  }
  updateSelf(controlData);

  buzz1.step();
}