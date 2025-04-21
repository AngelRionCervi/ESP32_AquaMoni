#pragma once
#include <Arduino.h>
#include "ScheduleButton.h"

ScheduleButton::ScheduleButton(int _buttonPin,
                               int _ledGreenPin,
                               int _ledRedPin) {
  buttonPin = _buttonPin;
  ledGreenPin = _ledGreenPin;
  ledRedPin = _ledRedPin;
  state = false;
  debouncer = ADebouncer();
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledGreenPin, OUTPUT);
  pinMode(ledRedPin, OUTPUT);
}

void ScheduleButton::setState(bool _state) {
  state = _state;

  if (!state) {
    scheduleOnStartTime = DateTime.now();
  } else {
    scheduleOnStartTime = 0;
  }

  if (!state) {
    digitalWrite(ledRedPin, HIGH);
    digitalWrite(ledGreenPin, LOW);
  } else {
    digitalWrite(ledRedPin, LOW);
    digitalWrite(ledGreenPin, HIGH);
  }
}

bool ScheduleButton::checkButton() {
  debouncer.debounce(digitalRead(buttonPin));
  if (debouncer.falling()) {
    Serial.println("toggle schedules: " + String(state));
    this->setState(!state);

    return true;
  }

  return false;
}

bool ScheduleButton::getState() {
  return state;
}

void ScheduleButton::toggleState() {
  this->setState(!state);
}

int ScheduleButton::getScheduleOnStartTime() {
  return scheduleOnStartTime;
}
