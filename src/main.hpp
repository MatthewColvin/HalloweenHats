#pragma once

#include <Arduino.h>
#include <espnow.h>

static constexpr long timeBetweenBellboyErrands = 1000;

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
  led leds[14];
  buzzer buzzers[2];
};

struct Button {
    const uint8_t PIN;
    uint32_t numberKeyPresses;
    bool pressed;
};