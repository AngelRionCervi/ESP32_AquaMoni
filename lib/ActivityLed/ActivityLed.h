#pragma once
#include <Arduino.h>

class ActivityLed {
 public:
  int greenPin;
  int redPin;
  bool isBlinking = false;
  int updateLastMillis = 0;
  int blinkPeriode = 500;
  String state;
  String previousState;

 public:
  ActivityLed(int greenPin, int redPin);
  void setState(String state);
  void update();
};
