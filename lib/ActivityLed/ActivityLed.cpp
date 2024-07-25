#include "ActivityLed.h"

ActivityLed::ActivityLed(int _greenPin, int _redPin) {
  greenPin = _greenPin;
  redPin = _redPin;

  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);
}

void ActivityLed::setState(String _state) {
  state = _state;
}

void ActivityLed::update() {
  if (state == "ok" && previousState != "ok") {
    digitalWrite(greenPin, HIGH);
    digitalWrite(redPin, LOW);
    previousState = "ok";
  } else if (state == "error" && previousState != "error") {
    digitalWrite(greenPin, LOW);
    digitalWrite(redPin, HIGH);
    previousState = "error";
  } else if ((state == "okBlink" || state == "errorBlink") &&
             millis() - updateLastMillis > blinkPeriode) {
    updateLastMillis = millis();

    if (state == "okBlink") {
      digitalWrite(greenPin, !isBlinking);
      digitalWrite(redPin, LOW);
      previousState = "okBlink";
    } else if (state == "errorBlink") {
      digitalWrite(greenPin, LOW);
      digitalWrite(redPin, !isBlinking);
      previousState = "errorBlink";
    }

    isBlinking = !isBlinking;
  }
}
