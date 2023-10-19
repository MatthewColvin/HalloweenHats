#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "main.hpp"

uint8_t doorManAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t* mac, uint8_t *incomingData, uint8_t len) {
    HatControlData *myData = reinterpret_cast<HatControlData*>(incomingData);

     Serial.println("Got Data");
     Serial.println(myData->leds->blue);

    if( myData->leds->blue == 255 ) {
    digitalWrite(LED_BUILTIN, LOW);
    } else {
    digitalWrite(LED_BUILTIN, HIGH);
    }
}

void boardSetup() {
    pinMode(LED_BUILTIN, OUTPUT);
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);

  if( auto err = esp_now_add_peer(doorManAddress, ESP_NOW_ROLE_COMBO, 1, nullptr, 0); err != 0 ) {
    Serial.print("Error adding peer: ");
    Serial.println(err);
  }

  if( auto err = esp_now_register_recv_cb(OnDataRecv); err != 0 ) {
    Serial.print("Error setting callback: ");
    Serial.println(err);
  }

    digitalWrite(LED_BUILTIN, HIGH);

}

void boardLoop() {

}