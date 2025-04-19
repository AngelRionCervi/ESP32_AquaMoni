#include "SmartPlug.h"

void SmartPlug::setState(bool _state) {
  if (state == _state) {
    return;
  }
  HttpClient httpClient = HttpClient(wifiClient, address, port);

  String request = _state ? smartPlugRequests[smartPlugType].turnOn
                          : smartPlugRequests[smartPlugType].turnOff;
  state = _state;

  httpClient.get(request);
}

void SmartPlug::toggleState() {
  setState(!state);
}

JsonDocument SmartPlug::fetchInfo() {
  HttpClient httpClient = HttpClient(wifiClient, address, port);
  String request = smartPlugRequests[smartPlugType].getState;

  httpClient.get(request);
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();

  JsonDocument doc;

  if (statusCode != 200) {
    String errorMessage =
        String("[SmartPlug] (init) could not get response from: ") + name;
    Serial.println(errorMessage);
    Serial.print("Status code: ");
    Serial.println(statusCode);
    doc["isOnline"] = false;

    return doc;
  }

  doc["isOnline"] = true;

  JsonDocument responseDoc;
  deserializeJson(responseDoc, response);

  if (smartPlugType == PLUG_TASMOTA_NAME) {
    String isOn = responseDoc["POWER"].as<String>();
    state = isOn == "ON";
  } else if (smartPlugType == PLUG_SHELLY_S_NAME) {
    bool isOn = responseDoc["ison"].as<bool>();
    state = isOn;
  }

  doc["state"] = state;

  return doc;
}

void SmartPlug::init(const char* _address,
                     int _port,
                     WiFiClient _wifiClient,
                     String _name,
                     String _smartPlugType) {
  address = _address;
  port = _port;
  wifiClient = _wifiClient;
  name = _name;
  smartPlugType = _smartPlugType;

  JsonDocument plugInfo = this->fetchInfo();
  state = plugInfo["state"].as<bool>();
  isOnline = plugInfo["isOnline"].as<bool>();
  hasInit = true;
}
