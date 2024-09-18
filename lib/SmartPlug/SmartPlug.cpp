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

bool SmartPlug::fetchState() {
  HttpClient httpClient = HttpClient(wifiClient, address, port);
  String request = smartPlugRequests[smartPlugType].getState;

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

  if (smartPlugType == PLUG_TASMOTA_NAME) {
    String isOn = doc["POWER"].as<String>();
    state = isOn == "ON";
  } else if (smartPlugType == PLUG_SHELLY_S_NAME) {
    bool isOn = doc["ison"].as<bool>();
    state = isOn;
  }

  return state;
}

bool SmartPlug::init(const char* _address,
                     int _port,
                     WiFiClient _wifiClient,
                     String _name,
                     String _smartPlugType) {
  address = _address;
  port = _port;
  wifiClient = _wifiClient;
  name = _name;
  smartPlugType = _smartPlugType;

  this->fetchState();

  hasInit = true;

  return state;
}
