#pragma once
#include "ScheduleButton.h"
#include "Arduino.h"

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

void ScheduleButton::update(bool _state) {
  state = _state;
  if (state) {
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
    this->update(!state);
  }
  
  return state;
}
