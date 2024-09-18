#pragma once
#include <ADebouncer.h>
#include "SmartPlug.h"

class Device {
 public:
  SmartPlug smartPlug;
  ADebouncer debouncer;
  int ledPin;
  int buttonPin;
  unsigned int button;
  String name;
  String id;
  String smartPlugType;
  JsonDocument schedule;
  bool smartPlugState;

 public:
  Device(const char* _address,
         const char* _name,
         const char* _smartPlugType,
         const char* _id,
         int _ledPin,
         int _buttonPin,
         unsigned int _button,
         JsonDocument schedule,
         WiFiClient _wifiClient,
         int _port = 80);
  SmartPlug getSmartPlugInfo();
  bool fetchSmartPlugState();
  void setSmartPlugState(bool _state);
  void toggleSmartPlugState();
  void checkButton();
  void checkSchedule();
  void updateLed();
};