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
  String id;
  JsonDocument schedule;
  bool shellyState;

 public:
  Device(const char* _address,
         const char* _name,
         const char* _id,
         int _ledPin,
         int _buttonPin,
         unsigned int _button,
         JsonDocument schedule,
         WiFiClient _wifiClient,
         int _port = 80);
  ShellyPlug getShellyInfo();
  bool fetchShellyState();
  void setShellyState(bool _state);
  void toggleShellyState();
  void checkButton();
  void checkSchedule();
  void updateLed();
};