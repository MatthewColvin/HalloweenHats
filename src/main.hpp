#pragma once

#include <Arduino.h>
#include <espnow.h>

#define NUM_HAT_LEDS 14
#define NUM_HAT_BUZZERS 2

struct led {
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;
  uint8_t brightness = 0;
};

struct buzzer {
  uint8_t loudness = 0;
};

struct HatControlData {
  led leds[NUM_HAT_LEDS];
  buzzer buzzers[NUM_HAT_BUZZERS];
};

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
  uint32_t lastPressTime;
  uint32_t lastReleaseTime;
  bool lastReleaseHandled;
  // Returns how long the button was held last
  // will be invalid on the next button press
  uint32_t lastHeldTime() {
    if (lastPressTime < lastReleaseTime) {
      return lastReleaseTime - lastPressTime;
    }
    return 0;
  }
};