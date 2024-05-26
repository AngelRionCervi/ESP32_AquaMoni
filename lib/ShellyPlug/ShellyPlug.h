#pragma once
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>

class ShellyPlug {
private:
    WiFiClient wifiClient;

public:
    String address;
    bool state;
    int port = 80;
    bool hasInit = false;

public:
    void setState(bool _state);
    void toggleState();
    void init(const char* _address, int _port, WiFiClient _wifiClient);
};
