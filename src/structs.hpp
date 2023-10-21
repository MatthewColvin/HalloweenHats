#pragma once
#include <Arduino.h>

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
  void setRed() {
    red = 255;
    green = 0;
    blue = 0;
  }
  void setGreen() {
    red = 0;
    green = 255;
    blue = 0;
  }
};

struct CommunicationData {
  bool isRequestingAllowingEntryRoutine;
  bool isRequestingDenyingEntryRoutine;
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
