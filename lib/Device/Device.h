#pragma once
#include <ADebouncer.h>

#include "ShellyPlug.h"

class Device {
 public:
  ShellyPlug shelly;
  ADebouncer debouncer;
  int ledPin;
  int buttonPin;
  unsigned int button;
  String name;
  JsonDocument schedule;
  bool shellyState;

 public:
  Device(const char* _address,
         const char* _name,
         int _ledPin,
         int _buttonPin,
         unsigned int _button,
         JsonDocument schedule,
         WiFiClient _wifiClient,
         int _port = 80);
  ShellyPlug getShellyInfo();
  void setShellyState(bool _state);
  void toggleShellyState();
  void checkButton();
  void checkSchedule();
  void updateLed();
};