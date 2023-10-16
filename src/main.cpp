#include <Arduino.h>
#include <ESP8266WiFi.h>
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

// Doorman
unsigned long lastTimeBellBoyRanErand = 0;
HatControlData doorManData; // Controller of the 2 headsets
HatControlData bellBoyData; // Controlled by the 1 headset
uint8_t bellBoyAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Bellboy

void setup() {
  // Common
  Serial.begin(115200);

  // Doorman
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_send_cb(onDataSend);
  esp_now_add_peer(bellBoyAddress, ESP_NOW_ROLE_SLAVE, 1, nullptr, 0);

  // Bellboy
}

void loop() {
  // Doorman
  if (lastTimeBellBoyRanErand + timeBetweenBellboyErrands < millis()) {
    esp_now_send(bellBoyAddress, (uint8_t *)&bellBoyData, sizeof(bellBoyData));
  }
  updateSelf(doorManData);

  // Bellboy
}

void onDataSend(uint8 *mac_addr, uint8_t sentStatus) {}

void updateSelf(HatControlData &aControlData) {
  // Update the current state of device based on ControlData
}