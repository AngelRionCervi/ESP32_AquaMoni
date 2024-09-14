#pragma once
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <map>

struct SmartPlugRequests {
  String getState;
  String turnOn;
  String turnOff;
};

static SmartPlugRequests tasmotaRequests = {
  getState : "/cm?cmnd=Power",
  turnOn : "/cm?cmnd=Power%20on",
  turnOff : "/cm?cmnd=Power%20off",
};

static SmartPlugRequests shellyRequests = {
  getState : "/relay/0",
  turnOn : "/relay/0?turn=on",
  turnOff : "/relay/0?turn=off",
};

static std::map<String, SmartPlugRequests> smartPlugRequests = {
    {"tasmota", tasmotaRequests},
    {"shelly", shellyRequests},
};

class SmartPlug {
 private:
  WiFiClient wifiClient;

 public:
  String address;
  String name;
  String plugType;
  bool state;
  int port = 80;
  bool hasInit = false;

 public:
  void setState(bool _state);
  void toggleState();
  bool fetchState();
  bool init(const char* _address,
            int _port,
            WiFiClient _wifiClient,
            String _name,
            String _plugType);
};
