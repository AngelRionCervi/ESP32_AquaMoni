//#pragma once

// void setLedOff() {
//   digitalWrite(LED_GREEN_PIN, LOW);
//   digitalWrite(LED_RED_PIN, LOW);
// }

// void setLedGreen() {
//   digitalWrite(LED_GREEN_PIN, HIGH);
//   digitalWrite(LED_RED_PIN, LOW);
// }

// void setLedRed() {
//   digitalWrite(LED_GREEN_PIN, LOW);
//   digitalWrite(LED_RED_PIN, HIGH);
// }

// void toggleLedRed() {
//   digitalWrite(LED_GREEN_PIN, LOW);
//   digitalWrite(LED_RED_PIN, digitalRead(LED_RED_PIN) == LOW ? HIGH : LOW);
// }

// void blinkGreen() {
//   setLedOff();
//   delay(400);
//   setLedGreen();
//   delay(150);
//   setLedOff();
//   delay(150);
//   setLedGreen();
// }

// void connectToWifiNetwork(const char* ssid, const char* password) {
//   WiFi.begin(ssid, password);

//   while (WiFi.status() != WL_CONNECTED) {
//     Serial.print("Attempting to connect to SSID: ");
//     Serial.println(ssid);

//     setLedGreen();
//     delay(1000);
//     setLedOff();
//     delay(1000);
//   }

//   Serial.print("Connected to: ");
//   Serial.println(ssid);
//   setLedGreen();
// }

#include "Arduino.h"

float truncateFloat(float input);

float decimalRound(float input, int decimals);

String* splitString(String& v, char delimiter, int& length);