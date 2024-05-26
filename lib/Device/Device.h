#pragma once
#include <ADebouncer.h>

#include "ShellyPlug.h"

class Device {
public:
  ShellyPlug shelly;
  ADebouncer debouncer;
  int ledPin;
  int buttonPin;
  String name;
  JsonDocument schedule;

public:
  Device(const char *_address, const char *_name, int _ledPin, int _buttonPin,
         JsonDocument schedule, WiFiClient _wifiClient, int _port = 80);
  ShellyPlug getShellyInfo();
  void setShellyState(bool _state);
  void checkButton();
  void checkSchedule();
};