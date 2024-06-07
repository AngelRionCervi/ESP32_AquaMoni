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

  debouncer = ADebouncer();
  debouncer.mode(DELAYED, 10, LOW);

  shelly = ShellyPlug();
  shelly.init(_address, _port, _wifiClient, name);
}

ShellyPlug Device::getShellyInfo() {
  return shelly;
}

void Device::setShellyState(bool state) {
  shelly.setState(state);
}

void Device::toggleShellyState() {
  shelly.toggleState();
}

void Device::checkButton() {
  debouncer.debounce(digitalRead(buttonPin));
  if (debouncer.falling()) {
    Serial.print("toggle: ");
    Serial.println(name);
    //shelly.toggleState();
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
