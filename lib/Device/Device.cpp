#include "Device.h"
#include <ESPDateTime.h>

Device::Device(const char* _address,
               const char* _name,
               int _ledPin,
               int _buttonPin,
               unsigned int _button,
               JsonDocument _schedule,
               WiFiClient _wifiClient,
               int _port) {
  name = _name;
  ledPin = _ledPin;
  buttonPin = _buttonPin;
  schedule = _schedule;
  button = _button;

  pinMode(_buttonPin, INPUT_PULLUP);
  pinMode(_ledPin, OUTPUT);

  debouncer = ADebouncer();
  debouncer.mode(DELAYED, 10, LOW);

  shelly = ShellyPlug();
  bool initState = shelly.init(_address, _port, _wifiClient, name);
  shellyState = initState;
  digitalWrite(ledPin, shellyState);
}

ShellyPlug Device::getShellyInfo() {
  return shelly;
}

void Device::setShellyState(bool state) {
  shellyState = state;
  shelly.setState(state);
  this->updateLed();
}

void Device::toggleShellyState() {
  shellyState = !shellyState;
  shelly.setState(shellyState);
  this->updateLed();
}

void Device::checkButton() {
  debouncer.debounce(digitalRead(buttonPin));
  if (debouncer.falling()) {
    Serial.print("toggle: ");
    Serial.println(name);
    shellyState = !shellyState;
    shelly.setState(shellyState);
    this->updateLed();
  }
}

void Device::updateLed() {
  if (shellyState) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void Device::checkSchedule() {
  if (schedule.is<bool>()) {
    bool scheduleAsBool = schedule.as<bool>();
    shelly.setState(scheduleAsBool);
  } else if (schedule.is<JsonArray>()) {
    JsonArrayConst scheduleAsArray = schedule.as<JsonArrayConst>();

    unsigned int start = scheduleAsArray[0].as<unsigned int>();
    unsigned int end = scheduleAsArray[1].as<unsigned int>();

    DateTimeParts dateParts = DateTime.getParts();
    unsigned int hours = dateParts.getHours();
    unsigned int minutes = dateParts.getMinutes();

    unsigned int fullDayMinutes = (hours * 60) + minutes;

    shelly.setState(fullDayMinutes >= start && fullDayMinutes <= end);
  }
}
