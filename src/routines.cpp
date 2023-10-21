#include "routines.hpp"
#include <Buzzer.h>

Buzzer buzz1(D7);
HatControlData controlData;

bool isDoingAllowRoutine = false;
bool isDoingDenyRoutine = false;
unsigned long int routineStart = 0;

auto denyToneNoteLength = 80;
auto denyToneNoteCycleLength = denyToneNoteLength * 3;
uint16_t x = denyToneNoteLength;
Buzzer::Melody_t denyTone{
    .nbNotes = 24,
    .duration = {x, x, x, x, x, x, x, x, x, x, x, x,
                 x, x, x, x, x, x, x, x, x, x, x},
    .frequency = {
        E4_NOTE_FREQ, A4_NOTE_FREQ, G5_NOTE_FREQ, E4_NOTE_FREQ, A4_NOTE_FREQ,
        G5_NOTE_FREQ, E4_NOTE_FREQ, A4_NOTE_FREQ, G5_NOTE_FREQ, E4_NOTE_FREQ,
        A4_NOTE_FREQ, G5_NOTE_FREQ, E4_NOTE_FREQ, A4_NOTE_FREQ, G5_NOTE_FREQ,
        E4_NOTE_FREQ, A4_NOTE_FREQ, G5_NOTE_FREQ, E4_NOTE_FREQ, A4_NOTE_FREQ,
        G5_NOTE_FREQ, E4_NOTE_FREQ, A4_NOTE_FREQ, G5_NOTE_FREQ, E4_NOTE_FREQ,
        A4_NOTE_FREQ, G5_NOTE_FREQ, E4_NOTE_FREQ, A4_NOTE_FREQ, G5_NOTE_FREQ}};

auto acceptToneLength = 50 + 50 + 100 + 200;
Buzzer::Melody_t acceptTone{
    .nbNotes = 4,
    .duration = {50, 50, 100, 200},
    .frequency = {F5_NOTE_FREQ, G5_NOTE_FREQ, A5_NOTE_FREQ, B5_NOTE_FREQ}};

////////////////////////////////////////////////////////////
unsigned long int lastStripUpdate = 0;
constexpr auto ledStripUpdateRate = 100;
void SendLEDStripUpdate() {}

//////////////////////////////////////////////////////////////////

void HandleRoutines() {
  if (communicationData.isRequestingAllowingEntryRoutine) {
    doAllowEntryRoutineUpdate();
  } else if (communicationData.isRequestingDenyingEntryRoutine) {
    doDenyEntryRoutineUpdate();
  } else {
    if (buzz1.hasMelody()) {
      buzz1.setMelody(nullptr);
    }
    doIdle();
  }

  if (auto currentTime = millis();
      lastStripUpdate + ledStripUpdateRate < currentTime) {
    SendLEDStripUpdate();
    lastStripUpdate = currentTime;
  }

  buzz1.step();
}

void doAllowEntryRoutineUpdate() {
  if (!isDoingAllowRoutine) {
    Serial.println("AllowEntry!!");
    routineStart = millis();
    buzz1.setMelody(&acceptTone);
    buzz1.unmute();
    isDoingAllowRoutine = true;
  }
  if (buzz1.hasMelody()) {
    // Ran while doing deny Routine
  } else {
    if (isDoingAllowRoutine) {
      Serial.println("Ended Allow Entry!");
      communicationData.isRequestingAllowingEntryRoutine = false;
      isDoingAllowRoutine = false;
    }
  }
}

void doDenyEntryRoutineUpdate() {
  if (!isDoingDenyRoutine) {
    // Called once for setup
    Serial.println("Deny Entry!!");
    routineStart = millis();
    buzz1.setMelody(&denyTone);
    buzz1.unmute();
    isDoingDenyRoutine = true;
  }
  if (buzz1.hasMelody()) {
    // Ran while doing deny Routine
    // Set leds to red and blink at rate of tone cycle
    auto timeIntoRoutine = millis() - routineStart;
    auto ledBrightness = map(timeIntoRoutine % denyToneNoteCycleLength, 0,
                             denyToneNoteCycleLength, 0, 255);
    for (int i = 0; i < NUM_HAT_LEDS; i++) {
      controlData.leds[i].setRed();
      controlData.leds[i].brightness = ledBrightness;
    }
  } else {
    // Clean up Setup Routine if its setup and end routine
    if (isDoingDenyRoutine) {
      Serial.println("Ended Deny Entry!");
      communicationData.isRequestingDenyingEntryRoutine = false;
      isDoingDenyRoutine = false;
    }
  }
}

constexpr auto halfBreathTime = 1000;
uint32_t numIdleCycles = 0;
void doIdle() {
  auto currentTime = millis();
  numIdleCycles = currentTime / halfBreathTime;
  auto currentPortion = currentTime % halfBreathTime;

  bool isBreathOut = numIdleCycles % 2 == 0;

  for (int i = 0; i < NUM_HAT_LEDS; i++) {
    controlData.leds[i].setWhite();
    auto value = 255 - map(currentPortion, 0, halfBreathTime, 0, 255);
    auto brightness = isBreathOut ? 255 - value : value;
    controlData.leds[i].brightness = brightness;
  }
}

void cancelRoutines() {
  communicationData.isRequestingAllowingEntryRoutine = false;
  communicationData.isRequestingDenyingEntryRoutine = false;
}
