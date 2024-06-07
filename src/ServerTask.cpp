#include "ServerTask.h"

void handleLast() {
  if (!isAuthentified()) {
    return;
  }

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
  if (!isAuthentified()) {
    return;
  }

  if (server.argName(0) != "r") {
    handleNotFound();
  }

  String rootPath = "/historical/";
  String argValue = server.arg(0);

  int daysLength;

  String* days = splitString(argValue, ',', daysLength);

  if (daysLength > 7) {
    server.send(500, "text/plain", "Payload too big !");

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
  if (!isAuthentified()) {
    return;
  }

  JsonDocument newConfigJson = deserializePost(server.arg(0));

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
  ESP.restart();
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
  if (!isAuthentified()) {
    return;
  }

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
  if (!isAuthentified()) {
    return;
  }

  areSchedulesDisabled = !areSchedulesDisabled;

  JsonDocument donePayloadJson;
  donePayloadJson["status"] = "success";
  donePayloadJson["newState"] = areSchedulesDisabled;

  String donePayloadString;
  serializeJson(donePayloadJson, donePayloadString);

  server.send(200, "application/json", donePayloadString);

  return;
}

void handleGetDevices() {
  if (!isAuthentified()) {
    return;
  }

  JsonDocument devicesJson;
  JsonArray devicesStateArray = devicesJson.to<JsonArray>();

  for (auto& [name, device] : devices) {
    JsonDocument deviceJson;
    ShellyPlug deviceShelly = device.getShellyInfo();
    deviceJson["name"] = name;
    deviceJson["state"] = deviceShelly.state;
    deviceJson["isOnline"] = deviceShelly.hasInit;
    deviceJson["schedule"] = device.schedule;
    deviceJson["button"] = device.button;
    devicesStateArray.add(deviceJson);
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
  if (!isAuthentified()) {
    return;
  }

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
  if (!isAuthentified()) {
    return;
  }

  JsonDocument getScheduleStateResponseJson;
  getScheduleStateResponseJson["data"] = areSchedulesDisabled;
  getScheduleStateResponseJson["status"] = "success";

  String getScheduleStateResponseString;
  serializeJson(getScheduleStateResponseJson, getScheduleStateResponseString);

  server.send(200, "application/json", getScheduleStateResponseString);

  return;
}

void handleLogin() {
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", String(serverAuthCookie) + "=0");
    return;
  }
  if (server.arg(0)) {
    JsonDocument authBody = deserializePost(server.arg(0));

    if (authBody["pass"] == serverPass) {
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", String(serverAuthCookie) + "=1");
      Serial.println("Log in Successful");
      server.send(200, "text/html", "Log in Successful !");
      return;
    }
    Serial.println("Log in Failed");
    server.send(401, "text/html", "Log in Failed !");
  }

  server.send(401, "text/html", "No credentials provided !");

  return;
}

void handlePing() {
  if (!isAuthentified()) {
    return;
  }

  JsonDocument pingJson;
  pingJson["status"] = "success";
  pingJson["data"] = "Online and authentified !";

  String pingString;
  serializeJson(pingJson, pingString);
  server.send(200, "application/json", pingString);

  return;
}

void ServerTaskCode(void* pvParameters) {
  Serial.print("ServerTask running on core ");
  Serial.println(xPortGetCoreID());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  Serial.print("Server address: ");
  Serial.println(WiFi.localIP());

  server.on("/ping", HTTP_GET, handlePing);
  server.on("/login", HTTP_POST, handleLogin);
  server.on("/updateconfig", HTTP_POST, handleUpdateConfig);
  server.on("/last", HTTP_GET, handleLast);
  server.on("/historical", HTTP_GET, handleHistorical);
  server.on("/toggledevice", HTTP_GET, handleDeviceManualToggle);
  server.on("/toggleschedule", HTTP_GET, handleScheduleManualToggle);
  server.on("/getconfig", HTTP_GET, handleGetConfig);
  server.on("/getdevices", HTTP_GET, handleGetDevices);

  server.onNotFound(handleNotFound);

  const char* headerKeys[] = {"Cookie"};
  size_t headerKeysSize = sizeof(headerKeys) / sizeof(char*);
  server.collectHeaders(headerKeys, headerKeysSize);

  server.begin();
  Serial.println("HTTP server started");

  for (;;) {
    server.handleClient();
    checkDevices();
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

bool isAuthentified() {
  if (server.hasHeader("Cookie")) {
    String cookie = server.header("Cookie");

    if (cookie.indexOf(String(serverAuthCookie) + "=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");

  JsonDocument failedAuthJson;
  failedAuthJson["status"] = "error";
  failedAuthJson["data"] = "Not authentified !";

  String failedAuthString;
  serializeJson(failedAuthJson, failedAuthString);
  server.send(403, "application/json", failedAuthString);

  return false;
}

JsonDocument deserializePost(String body) {
  JsonDocument jsonResponse;
  DeserializationError err = deserializeJson(jsonResponse, body);

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

  return jsonResponse;
}
