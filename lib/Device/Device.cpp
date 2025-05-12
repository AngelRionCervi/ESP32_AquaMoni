#include "Device.h"
#include <ESPDateTime.h>

Device::Device(const char* _address,
               const char* _name,
               const char* _smartPlugType,
               const char* _id,
               int _ledPin,
               int _buttonPin,
               unsigned int _button,
               JsonDocument _schedule,
               WiFiClient _wifiClient,
               int _port) {
  name = _name;
  id = _id;
  ledPin = _ledPin;
  buttonPin = _buttonPin;
  schedule = _schedule;
  button = _button;
  smartPlugType = _smartPlugType;

  pinMode(_buttonPin, INPUT_PULLUP);
  pinMode(_ledPin, OUTPUT);

  debouncer = ADebouncer();
  debouncer.mode(DELAYED, 10, LOW);

  smartPlug = SmartPlug();
  smartPlug.init(_address, _port, _wifiClient, _name, _smartPlugType);
  smartPlugState = smartPlug.state;
  smartPlugOnline = smartPlug.isOnline;
  digitalWrite(ledPin, smartPlugState);
}

SmartPlug Device::getSmartPlugInfo() {
  return smartPlug;
}

void Device::setSmartPlugState(bool state) {
  smartPlugState = state;
  smartPlug.setState(state);
  this->updateLed();
}

void Device::toggleSmartPlugState() {
  smartPlugState = !smartPlugState;
  smartPlug.setState(smartPlugState);
  this->updateLed();
}

bool Device::checkButton() {
  debouncer.debounce(digitalRead(buttonPin));
  if (debouncer.falling()) {
    smartPlugState = !smartPlugState;
    smartPlug.setState(smartPlugState);
    this->updateLed();

    return true;
  }

  return false;
}

void Device::updateLed() {
  if (smartPlugState) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

void Device::checkSchedule() {
  if (schedule.is<bool>()) {
    bool scheduleAsBool = schedule.as<bool>();
    smartPlugState = scheduleAsBool;
    smartPlug.setState(smartPlugState);
  } else if (schedule.is<JsonArray>()) {
    JsonArrayConst scheduleAsArray = schedule.as<JsonArrayConst>();

    unsigned int start = scheduleAsArray[0].as<unsigned int>();
    unsigned int end = scheduleAsArray[1].as<unsigned int>();

    DateTimeParts dateParts = DateTime.getParts();
    unsigned int hours = dateParts.getHours();
    unsigned int minutes = dateParts.getMinutes();

    unsigned int fullDayMinutes = (hours * 60) + minutes;

    smartPlugState = fullDayMinutes >= start && fullDayMinutes <= end;
    smartPlug.setState(smartPlugState);
  }
  this->updateLed();
}

bool Device::fetchSmartPlugState() {
  JsonDocument smartPlugInfo = smartPlug.fetchInfo();
  smartPlugState = smartPlugInfo["state"].as<bool>();
  this->updateLed();
  return smartPlugState;
}
