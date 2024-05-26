#include "ShellyPlug.h"

void ShellyPlug::setState(bool _state) {
  if (state == _state) {
    return;
  }
  HttpClient httpClient = HttpClient(wifiClient, address, port);

  String stateParam = _state ? "on" : "off";
  String request = String("/relay/0?turn=") + stateParam;
  state = _state;

  httpClient.get(request);
}

void ShellyPlug::toggleState() {
  setState(!state);
}

void ShellyPlug::init(const char* _address, int _port, WiFiClient _wifiClient) {
  address = _address;
  port = _port;
  wifiClient = _wifiClient;

  HttpClient httpClient = HttpClient(wifiClient, address, port);
  String request = String("/relay/0");

  httpClient.get(request);
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();

  if (statusCode != 200) {
    Serial.println("[ShellyPlug] (init) could not get response");
    Serial.print("Status code: ");
    Serial.println(statusCode);

    return;
  }

  JsonDocument doc;
  deserializeJson(doc, response);

  bool isOn = doc["ison"].as<bool>();

  state = isOn;
  hasInit = true;
}
