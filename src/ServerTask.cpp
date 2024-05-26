#include "ServerTask.h"

void loadConfig() {
  File fileConfig = SD.open(FILE_CONFIG, "r");

  if (!fileConfig) {
    Serial.println("Could not open config.jso");
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

    Device newDevice(ip, name, ledMap[button], buttonMap[button], scheduleCopy, wifiClient);
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

  server.send(200, "application/json", content);
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

  server.send(200, "text/plain", content);

  delete[] days;

  return;
}

void handleUpdateConfig() {
  if (server.argName(0) != "pass" || server.arg(0) != CONFIG_PASS) return;

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

  if (err.code() != DeserializationError::Ok) return;

  File fileConfig = SD.open(FILE_CONFIG, "w");

  if (!fileConfig) {
    server.send(500, "text/plain", "Could not open config.jso for updating");
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
  notFoundJson["m"] = "route not found";
  notFoundJson["s"] = 404;

  String notFoundString;
  serializeJson(notFoundJson, notFoundString);

  server.send(200, "application/json", notFoundString);

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

  server.on("/last", handleLast);
  server.on("/historical", handleHistorical);
  server.on("/updateconfig", HTTP_POST, handleUpdateConfig);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

  for (;;) {
    server.handleClient();
    checkDevices();
    delay(2);  //allow the cpu to switch to other tasks
  }
}

void checkDevices() {
  int updateMillis = false;

  for (auto& [name, device] : devices) {
    device.checkButton();
    if (!areSchedulesDisabled && millis() - scheduleUpdateLastMillis > scheduleUpdatePeriode) {
      device.checkSchedule();
      updateMillis = true;
    }
  }

  if (updateMillis) {
    scheduleUpdateLastMillis = millis();
  }
}
