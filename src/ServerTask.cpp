#include "ServerTask.h"

void loadConfig() {
  File fileConfig = SD.open(FILE_CONFIG, "r");

  if (!fileConfig) {
    Serial.println("Could not open config.jso [loadConfig]");
  }

  String configString;
  while (fileConfig.available()) {
    configString += fileConfig.readString();
  }

  JsonDocument configJson;
  deserializeJson(configJson, configString);

  JsonArray deviceArray = configJson["devices"];

  for (int i = 0; i < deviceArray.size(); i++) {
    JsonObject deviceObj = deviceArray[i];
    const char* name = deviceObj["name"].as<const char*>();
    const char* ip = deviceObj["ip"].as<const char*>();
    unsigned int button = deviceObj["button"].as<unsigned int>();
    JsonVariant schedule = deviceObj["schedule"].as<JsonVariant>();

    JsonDocument scheduleCopy = schedule;

    Device newDevice(ip, name, ledMap[button], buttonMap[button], scheduleCopy,
                     wifiClient);
    devices.emplace(name, newDevice);
  }
}

void handleLast() {
  File fileLast = SD.open(FILE_LAST, "r");
  if (!fileLast) {
    Serial.println("Could not open last.jso for sending");
  }

  String content;
  while (fileLast.available()) {
    content += fileLast.readString();
  }

  JsonDocument lastJson;
  deserializeJson(lastJson, content);

  JsonDocument getLastResponseJson;
  getLastResponseJson["data"] = lastJson;
  getLastResponseJson["status"] = "success";

  String getLastResponseString;
  serializeJson(getLastResponseJson, getLastResponseString);

  server.send(200, "application/json", getLastResponseString);
  fileLast.close();
}

void handleHistorical() {
  if (server.argName(0) != "r") {
    handleNotFound();
  }

  String rootPath = "/historical/";
  String argValue = server.arg(0);

  int daysLength;

  String* days = splitString(argValue, ',', daysLength);

  if (daysLength > 7) {
    server.send(500, "text/plain", "payload too big");

    return;
  }

  String content = "";
  for (int i = 0; i < daysLength; i++) {
    String fileName = "/historical/";
    fileName += days[i];
    fileName += ".jso";

    Serial.println("fileName");
    Serial.println(fileName);

    File dayFile = SD.open(fileName, "r");

    if (!dayFile) {
      Serial.println("");
      Serial.print("Could not open: ");
      Serial.println(fileName);
    }

    while (dayFile.available()) {
      content += dayFile.readString();
    }

    dayFile.close();
    Serial.println(days[i]);
  }

  JsonDocument getHistoricalJson;
  getHistoricalJson["data"] = content;
  getHistoricalJson["status"] = "success";

  String getHistoricalString;
  serializeJson(getHistoricalJson, getHistoricalString);

  server.send(200, "application/json", getHistoricalString);

  delete[] days;

  return;
}

void handleUpdateConfig() {
  if (server.argName(0) != "pass" || server.arg(0) != CONFIG_PASS) {
    handleNotFound();
    return;
  };

  JsonDocument newConfigJson;
  DeserializationError err = deserializeJson(newConfigJson, server.arg(1));

  switch (err.code()) {
    case DeserializationError::Ok:
      break;
    case DeserializationError::InvalidInput:
      server.send(500, "text/plain", "Invalid input");
      break;
    case DeserializationError::NoMemory:
      server.send(500, "text/plain", "Not enough memory");
      break;
    default:
      server.send(500, "text/plain", "Deserialization failed");
      break;
  }

  if (err.code() != DeserializationError::Ok)
    return;

  File fileConfig = SD.open(FILE_CONFIG, "w");

  if (!fileConfig) {
    server.send(500, "text/plain",
                "Could not open config.jso [handleUpdateConfig]");
    return;
  }

  serializeJson(newConfigJson, fileConfig);
  fileConfig.close();

  server.send(200, "text/plain", "Config updated, resetting...");
  Serial.println("Config updated");
  resetFunc();
}

void handleNotFound() {
  JsonDocument notFoundJson;
  notFoundJson["message"] = "route not found";
  notFoundJson["status"] = "error";

  String notFoundString;
  serializeJson(notFoundJson, notFoundString);

  server.send(404, "application/json", notFoundString);

  return;
}

void handleDeviceManualToggle() {
  if (server.argName(0) != "name") {
    handleNotFound();
    return;
  }

  if (!areSchedulesDisabled) {
    JsonDocument donePayloadJson;
    donePayloadJson["status"] = "error";
    donePayloadJson["message"] = "Schedules not disabled !";
  }

  std::string deviceName = server.arg(0).c_str();
  Device& device = devices.at(deviceName);

  device.toggleShellyState();

  bool newState = device.getShellyInfo().state;

  JsonDocument donePayloadJson;
  donePayloadJson["status"] = "success";
  donePayloadJson["newState"] = newState;

  String donePayloadString;
  serializeJson(donePayloadJson, donePayloadString);

  server.send(200, "application/json", donePayloadString);

  return;
}

void handleScheduleManualToggle() {
  areSchedulesDisabled = !areSchedulesDisabled;

  JsonDocument donePayloadJson;
  donePayloadJson["status"] = "success";
  donePayloadJson["newState"] = areSchedulesDisabled;

  String donePayloadString;
  serializeJson(donePayloadJson, donePayloadString);

  server.send(200, "application/json", donePayloadString);

  return;
}

void handleGetDevicesState() {
  JsonDocument devicesStateJson;
  JsonArray devicesStateArray = devicesStateJson.to<JsonArray>();

  for (auto& [name, device] : devices) {
    JsonDocument deviceState;
    bool state = device.getShellyInfo().state;
    deviceState["name"] = name;
    deviceState["state"] = state;
    devicesStateArray.add(deviceState);
  }

  JsonDocument devicesStateResponseJson;
  devicesStateResponseJson["data"] = devicesStateArray;
  devicesStateResponseJson["status"] = "success";

  String getDevicesStateResponseString;
  serializeJson(devicesStateResponseJson, getDevicesStateResponseString);

  server.send(200, "application/json", getDevicesStateResponseString);

  return;
}

void handleGetConfig() {
  File fileConfig = SD.open(FILE_CONFIG, "r");

  if (!fileConfig) {
    Serial.println("Could not open config.jso in [handleGetConfig]");
  }

  String configString;
  while (fileConfig.available()) {
    configString += fileConfig.readString();
  }

  JsonDocument configJson;
  deserializeJson(configJson, configString);

  JsonDocument getConfigResponseJson;
  getConfigResponseJson["data"] = configJson;
  getConfigResponseJson["status"] = "success";

  String getConfigResponseString;
  serializeJson(getConfigResponseJson, getConfigResponseString);

  server.send(200, "application/json", getConfigResponseString);

  return;
}

void handleGetScheduleState() {
  JsonDocument getScheduleStateResponseJson;
  getScheduleStateResponseJson["data"] = areSchedulesDisabled;
  getScheduleStateResponseJson["status"] = "success";

  String getScheduleStateResponseString;
  serializeJson(getScheduleStateResponseJson, getScheduleStateResponseString);

  server.send(200, "application/json", getScheduleStateResponseString);

  return;
}

void ServerTaskCode(void* pvParameters) {
  Serial.print("ServerTask running on core ");
  Serial.println(xPortGetCoreID());

  loadConfig();

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  Serial.print("Server address: ");
  Serial.println(WiFi.localIP());

  server.on("/last", HTTP_GET, handleLast);
  server.on("/historical", HTTP_GET, handleHistorical);
  server.on("/toggledevice", HTTP_GET, handleDeviceManualToggle);
  server.on("/toggleschedule", HTTP_GET, handleScheduleManualToggle);
  server.on("/getconfig", HTTP_GET, handleGetConfig);
  server.on("/getdevicesstate", HTTP_GET, handleGetDevicesState);
  server.on("/updateconfig", HTTP_POST, handleUpdateConfig);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  for (;;) {
    server.handleClient();
    // checkDevices();
    delay(2);  // allow the cpu to switch to other tasks
  }
}

void checkDevices() {
  int updateMillis = false;

  for (auto& [name, device] : devices) {
    device.checkButton();
    if (!areSchedulesDisabled &&
        millis() - scheduleUpdateLastMillis > scheduleUpdatePeriode) {
      device.checkSchedule();
      updateMillis = true;
    }
  }

  if (updateMillis) {
    scheduleUpdateLastMillis = millis();
  }
}
