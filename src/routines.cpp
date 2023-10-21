#include "routines.hpp"
#include <Buzzer.h>

Buzzer buzz1(D7);
HatControlData controlData;

bool isAllowRoutineSetup = false;
bool isDenyRoutineSetup = false;

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

//////////////////////////////////////////////////////////////////

void HandleRoutines() {
  if (communicationData.isDoingAllowingEntryRoutine) {
    doAllowEntryRoutineUpdate(controlData);
  } else if (communicationData.isDoingDenyingEntryRoutine) {
    doDenyEntryRoutineUpdate(controlData);
  } else {
    if (buzz1.hasMelody()) {
      buzz1.setMelody(nullptr);
    }
    doIdle(controlData);
  }
  updateSelf(controlData);

  buzz1.step();
}

void doAllowEntryRoutineUpdate(HatControlData &aControlData) {
  if (!isAllowRoutineSetup) {
    Serial.println("AllowEntry!!");
    buzz1.setMelody(&acceptTone);
    buzz1.unmute();
    isAllowRoutineSetup = true;
  }
  if (buzz1.hasMelody()) {
    // Ran while doing deny Routine
  } else {
    if (isAllowRoutineSetup) {
      Serial.println("Ended Allow Entry!");
      communicationData.isDoingAllowingEntryRoutine = false;
      isAllowRoutineSetup = false;
    }
  }
}

void doDenyEntryRoutineUpdate(HatControlData &aControlData) {
  if (!isDenyRoutineSetup) {
    // Called once for setup
    Serial.println("Deny Entry!!");
    buzz1.setMelody(&denyTone);
    buzz1.unmute();
    isDenyRoutineSetup = true;
  }
  if (buzz1.hasMelody()) {
    // Ran while doing deny Routine

  } else {
    // Clean up Setup Routine if its setup and end routine
    if (isDenyRoutineSetup) {
      Serial.println("Ended Deny Entry!");
      communicationData.isDoingDenyingEntryRoutine = false;
      isDenyRoutineSetup = false;
    }
  }
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

void cancelRoutines() {
  communicationData.isDoingAllowingEntryRoutine = false;
  communicationData.isDoingDenyingEntryRoutine = false;
}

////////////////////////////////////////////////////////////

void updateSelf(HatControlData &aControlData) {}
