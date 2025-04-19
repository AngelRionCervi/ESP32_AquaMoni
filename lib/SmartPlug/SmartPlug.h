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

static const String PLUG_TASMOTA_NAME = "tasmota_plug";
static const String PLUG_SHELLY_S_NAME = "shelly_plug_s";

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
    {PLUG_TASMOTA_NAME, tasmotaRequests},
    {PLUG_SHELLY_S_NAME, shellyRequests},
};

class SmartPlug {
 private:
  WiFiClient wifiClient;

 public:
  String address;
  String name;
  String smartPlugType;
  bool state;
  bool isOnline;
  int port = 80;
  bool hasInit = false;

 public:
  void setState(bool _state);
  void toggleState();
  JsonDocument fetchInfo();
  void init(const char* _address,
            int _port,
            WiFiClient _wifiClient,
            String _name,
            String _smartPlugType);
};
