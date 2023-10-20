#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "main.hpp"

// Doorman
unsigned long lastTimeBellBoyRanErand = 0;
HatControlData doorManData; // Controller of the 2 headsets
HatControlData bellBoyData; // Controlled by the 1 headset
//variables to keep track of the timing of recent interrupts
unsigned long button_time = 0;
unsigned long last_button_time = 0;
uint8_t bellBoyAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

Button button1 = {D6, 0, false};

void onDataSend(uint8 *mac_addr, uint8_t sentStatus);
void updateSelf(HatControlData &aControlData);


void IRAM_ATTR button_press_isr() {
  button_time = millis();
  if (button_time - last_button_time > 250) {
    button1.numberKeyPresses++;
    button1.pressed = true;
    last_button_time = button_time;
  }
}

// Bellboy

void boardSetup() {
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, button_press_isr, FALLING);

  // Doorman
//   esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(onDataSend);
  int err = esp_now_add_peer(bellBoyAddress, ESP_NOW_ROLE_COMBO, 1, nullptr, 0);

  if( 0 == err ) {
    Serial.println("Added peer successfully");
  } else {
    Serial.print("Error adding peer: ");
    Serial.println(err);
  }
}

void boardLoop() {
  // Doorman
  if (last_button_time > lastTimeBellBoyRanErand) {
    bellBoyData.leds->blue = button1.numberKeyPresses > 0 ? bellBoyData.leds->blue == 255 ? 0 : 255 : 0;

    if( auto err = esp_now_send(bellBoyAddress, (uint8_t *)&bellBoyData, sizeof(bellBoyData)); err != 0 ) {
        Serial.print("Error adding peer: ");
        Serial.println(err);
    }

    lastTimeBellBoyRanErand = millis();
    button1.numberKeyPresses = 0;
  }
  updateSelf(doorManData);

  // Bellboy
}

void onDataSend(uint8 *mac_addr, uint8_t sentStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sentStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}

void updateSelf(HatControlData &aControlData) {
  // Update the current state of device based on ControlData
}