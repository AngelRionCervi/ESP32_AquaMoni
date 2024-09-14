#include "SmartPlug.h"

void SmartPlug::setState(bool _state) {
  if (state == _state) {
    return;
  }
  HttpClient httpClient = HttpClient(wifiClient, address, port);

  String request = _state ? smartPlugRequests[plugType].turnOn
                          : smartPlugRequests[plugType].turnOff;
  state = _state;

  httpClient.get(request);
}

void SmartPlug::toggleState() {
  setState(!state);
}

bool SmartPlug::fetchState() {
  HttpClient httpClient = HttpClient(wifiClient, address, port);
  String request = smartPlugRequests[plugType].getState;

  httpClient.get(request);
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();

  if (statusCode != 200) {
    String errorMessage =
        String("[SmartPlug] (init) could not get response from: ") + name;
    Serial.println(errorMessage);
    Serial.print("Status code: ");
    Serial.println(statusCode);

    return false;
  }

  JsonDocument doc;
  deserializeJson(doc, response);

  if (plugType == "tasmota") {
    String isOn = doc["POWER"].as<String>();
    state = isOn == "ON";
  } else if (plugType == "shelly") {
    bool isOn = doc["ison"].as<bool>();
    state = isOn;
  }

  return state;
}

bool SmartPlug::init(const char* _address,
                     int _port,
                     WiFiClient _wifiClient,
                     String _name,
                     String _plugType) {
  address = _address;
  port = _port;
  wifiClient = _wifiClient;
  name = _name;
  plugType = _plugType;

  this->fetchState();

  hasInit = true;

  return state;
}
