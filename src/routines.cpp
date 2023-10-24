#include "routines.hpp"
#include <Buzzer.h>
#include <FastLED.h>

#define NUM_LEDS 12

#define DATA_PIN D7
#define CLOCK_PIN D5

Buzzer buzz1(D8);
HatControlData controlData;

bool isDoingAllowRoutine = false;
bool isDoingDenyRoutine = false;
bool isDoingMarioRoutine = false;
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

Buzzer::Melody_t marioMainTheme{
    .nbNotes = 78,
    .duration =
        {
            12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,

            12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,

            9,  9,  9,  12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,

            12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,

            9,  9,  9,  12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        },
    .frequency = {
        NOTE_E7, NOTE_E7, 0,       NOTE_E7, 0,       NOTE_C7,  NOTE_E7, 0,
        NOTE_G7, 0,       0,       0,       NOTE_G6, 0,        0,       0,

        NOTE_C7, 0,       0,       NOTE_G6, 0,       0,        NOTE_E6, 0,
        0,       NOTE_A6, 0,       NOTE_B6, 0,       NOTE_AS6, NOTE_A6, 0,

        NOTE_G6, NOTE_E7, NOTE_G7, NOTE_A7, 0,       NOTE_F7,  NOTE_G7, 0,
        NOTE_E7, 0,       NOTE_C7, NOTE_D7, NOTE_B6, 0,        0,

        NOTE_C7, 0,       0,       NOTE_G6, 0,       0,        NOTE_E6, 0,
        0,       NOTE_A6, 0,       NOTE_B6, 0,       NOTE_AS6, NOTE_A6, 0,

        NOTE_G6, NOTE_E7, NOTE_G7, NOTE_A7, 0,       NOTE_F7,  NOTE_G7, 0,
        NOTE_E7, 0,       NOTE_C7, NOTE_D7, NOTE_B6, 0,        0}};

CRGB leds[NUM_LEDS];

////////////////////////////////////////////////////////////
unsigned long int lastStripUpdate = 0;
constexpr auto ledStripUpdateRate = 100;

void SendLEDStripUpdate() {
  for (uint8 i = 0; i < NUM_LEDS; i++) {
    leds[i].r = controlData.leds[i].red;
    leds[i].g = controlData.leds[i].green;
    leds[i].b = controlData.leds[i].blue;
    FastLED.setBrightness(controlData.leds[i].brightness);
  }
  FastLED.show();
}

//////////////////////////////////////////////////////////////////

void HandleRoutineSetup() {
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, NUM_LEDS);
}

void HandleRoutines() {
  if (communicationData.isRequestingAllowingEntryRoutine) {
    doAllowEntryRoutineUpdate();
  } else if (communicationData.isRequestingDenyingEntryRoutine) {
    doDenyEntryRoutineUpdate();
  } else if (communicationData.isRequestingMarioRoutine) {
    doMarioRoutineUpdate();
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
    // Ran while doing allow Routine
    // Set leds green for the accept routine
    for (int i = 0; i < NUM_HAT_LEDS; i++) {
      controlData.leds[i].setGreen();
      controlData.leds[i].brightness = 255;
    }
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

void doMarioRoutineUpdate() {
  if (!isDoingMarioRoutine) {
    Serial.println("Mario Entry!!");
    routineStart = millis();
    buzz1.setMelody(&marioMainTheme);
    buzz1.unmute();
    isDoingMarioRoutine = true;
  }
  if (buzz1.hasMelody()) {
    auto timeIntoRoutine = millis() - routineStart;
    auto cycleLength = 300;
    auto cycleNum = timeIntoRoutine / cycleLength;
    auto firstSetSwapped = cycleNum % 2 == 0;
    for (int i = 0; i < NUM_HAT_LEDS; i++) {
      if (i % 2 == 0) {
        if (firstSetSwapped) {
          controlData.leds[i].setRed();
        } else {
          controlData.leds[i].setWhite();
        }
      } else {
        if (firstSetSwapped) {
          controlData.leds[i].setWhite();
        } else {
          controlData.leds[i].setRed();
        }
      }
      controlData.leds[i].brightness = 255;
    }
  } else {
    if (isDoingMarioRoutine) {
      Serial.println("Ended Mario!");
      communicationData.isRequestingMarioRoutine = false;
      isDoingMarioRoutine = false;
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
    if (!communicationData.isLightsOnInIdle) {
      controlData.leds[i].brightness = 0;
    } else {
      controlData.leds[i].brightness = brightness;
    }
  }
}

void cancelRoutines() {
  communicationData.isRequestingAllowingEntryRoutine = false;
  communicationData.isRequestingDenyingEntryRoutine = false;
  communicationData.isRequestingMarioRoutine = false;
}
