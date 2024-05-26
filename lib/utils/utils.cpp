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

#include "utils.h"

float truncateFloat(float input) {
  int intValue = input * 100.0;

  return intValue / 100.0 + 0.05;
}

float decimalRound(float input, int decimals) {
  float scale = pow(10, decimals);
  return round(input * scale) / scale;
}

String* splitString(String& v, char delimiter, int& length) {
  length = 1;
  bool found = false;

  // Figure out how many items the array should have
  for (int i = 0; i < v.length(); i++) {
    if (v[i] == delimiter) {
      length++;
      found = true;
    }
  }

  // If the delimiter is found than create the array
  // and split the String
  if (found) {

    // Create array
    String* values = new String[length];

    // Split the string into array
    int i = 0;
    for (int itemIndex = 0; itemIndex < length; itemIndex++) {
      for (; i < v.length(); i++) {

        if (v[i] == delimiter) {
          i++;
          break;
        }
        values[itemIndex] += v[i];
      }
    }

    // Done, return the values
    return values;
  }

  // No delimiter found
  String* initVal = new String[1];
  initVal[0] = v;

  return initVal;
}
