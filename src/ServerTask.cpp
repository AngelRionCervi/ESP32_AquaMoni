// #include "ServerTask.h"

// void handleLast() {
//   if (!isAuthentified()) {
//     return;
//   }

//   File fileLast = SD.open(FILE_LAST, "r");
//   if (!fileLast) {
//     Serial.println("Could not open last.jso for sending");
//     activityLed.setState("errorBlink");
//   }

//   String content;
//   while (fileLast.available()) {
//     content += fileLast.readString();
//   }

//   JsonDocument lastJson;
//   deserializeJson(lastJson, content);

//   JsonDocument getLastResponseJson;
//   getLastResponseJson["data"] = lastJson;
//   getLastResponseJson["status"] = "success";

//   String getLastResponseString;
//   serializeJson(getLastResponseJson, getLastResponseString);

//   server.send(200, "application/json", getLastResponseString);
//   fileLast.close();
// }

// void handleHistorical() {
//   if (!isAuthentified()) {
//     return;
//   }

//   if (server.argName(0) != "r") {
//     handleNotFound();
//   }

//   String rootPath = "/historical/";
//   String argValue = server.arg(0);

//   int daysLength;

//   String* days = splitString(argValue, ',', daysLength);

//   if (daysLength > 62) {
//     server.send(500, "text/plain", "Payload too big !");

//     return;
//   }

//   server.sendHeader("Cache-Control", "no-cache, no-store, must-revalidate");
//   server.sendHeader("Expires", "-1");
//   server.setContentLength(CONTENT_LENGTH_UNKNOWN);

//   server.send(200, "text/plain", "");

//   for (int i = 0; i < daysLength; i++) {
//     String fileName = "/historical/";
//     fileName += days[i];
//     fileName += ".jso";

//     Serial.println("fileName");
//     Serial.println(fileName);

//     File dayFile = SD.open(fileName, "r");

//     if (!dayFile) {
//       Serial.println("");
//       Serial.print("Could not open: ");
//       Serial.println(fileName);
//       dayFile.close();
//     } else {
//       String content = "";

//       while (dayFile.available()) {
//         content += dayFile.readString();
//       }

//       server.sendContent(content);

//       dayFile.close();
//       // Serial.println(days[i]);
//     }
//   }

//   server.client().stop();

//   delete[] days;

//   return;
// }

// void handleUpdateConfig() {
//   if (!isAuthentified()) {
//     return;
//   }

//   JsonDocument newConfigJson = deserializePost(server.arg(0));

//   File fileConfig = SD.open(FILE_CONFIG, "w");

//   if (!fileConfig) {
//     JsonDocument noConfigJson;
//     noConfigJson["message"] = "Could not open config.jso [handleUpdateConfig]";
//     noConfigJson["status"] = "error";

//     String noConfigString;
//     serializeJson(noConfigJson, noConfigString);

//     server.send(500, "application/json", noConfigString);
//     return;
//   }

//   serializeJson(newConfigJson, fileConfig);
//   fileConfig.close();

//   JsonDocument updatedJson;
//   updatedJson["message"] = "Config updated, resetting...";
//   updatedJson["status"] = "success";

//   String updateString;
//   serializeJson(updatedJson, updateString);

//   server.send(200, "application/json", updateString);
//   Serial.println("Config updated");

//   return;
// }

// void handleNotFound() {
//   JsonDocument notFoundJson;
//   notFoundJson["message"] = "route not found";
//   notFoundJson["status"] = "error";

//   String notFoundString;
//   serializeJson(notFoundJson, notFoundString);

//   server.send(404, "application/json", notFoundString);

//   return;
// }

// void handleDeviceManualToggle() {
//   if (!isAuthentified()) {
//     return;
//   }

//   if (server.argName(0) != "id") {
//     handleNotFound();
//     return;
//   }

//   if (!scheduleButton.getState()) {
//     JsonDocument donePayloadJson;
//     donePayloadJson["status"] = "error";
//     donePayloadJson["message"] = "Schedules not disabled !";
//   }

//   std::string deviceId = server.arg(0).c_str();
//   Device& device = devices.at(deviceId);

//   device.toggleShellyState();

//   bool newState = device.shellyState;

//   JsonDocument donePayloadJson;
//   donePayloadJson["status"] = "success";
//   donePayloadJson["newState"] = newState;

//   String donePayloadString;
//   serializeJson(donePayloadJson, donePayloadString);

//   server.send(200, "application/json", donePayloadString);

//   return;
// }

// void handleScheduleManualToggle() {
//   if (!isAuthentified()) {
//     return;
//   }

//   scheduleButton.toggleState();

//   JsonDocument donePayloadJson;
//   donePayloadJson["status"] = "success";
//   donePayloadJson["newState"] = scheduleButton.getState();

//   String donePayloadString;
//   serializeJson(donePayloadJson, donePayloadString);

//   server.send(200, "application/json", donePayloadString);

//   return;
// }

// void handleGetDevices() {
//   if (!isAuthentified()) {
//     return;
//   }

//   JsonDocument devicesJson;
//   JsonArray devicesStateArray = devicesJson.to<JsonArray>();

//   for (auto& [id, device] : devices) {
//     JsonDocument deviceJson;
//     ShellyPlug deviceShelly = device.getShellyInfo();
//     deviceJson["name"] = device.name;
//     deviceJson["id"] = id;
//     deviceJson["state"] = deviceShelly.state;
//     deviceJson["isOnline"] = deviceShelly.hasInit;
//     deviceJson["schedule"] = device.schedule;
//     deviceJson["button"] = device.button;
//     devicesStateArray.add(deviceJson);
//   }

//   JsonDocument devicesStateResponseJson;
//   devicesStateResponseJson["data"] = devicesStateArray;
//   devicesStateResponseJson["status"] = "success";

//   String getDevicesStateResponseString;
//   serializeJson(devicesStateResponseJson, getDevicesStateResponseString);

//   server.send(200, "application/json", getDevicesStateResponseString);

//   return;
// }

// void handleGetHardwareToggleUpdate() {
//   if (!isAuthentified()) {
//     return;
//   }

//   JsonDocument update;
//   JsonDocument devicesJson;
//   JsonArray devicesStateArray = devicesJson.to<JsonArray>();

//   for (auto& [id, device] : devices) {
//     JsonDocument deviceJson;
//     ShellyPlug deviceShelly = device.getShellyInfo();
//     deviceJson["name"] = device.name;
//     deviceJson["id"] = id;
//     deviceJson["state"] = deviceShelly.state;
//     deviceJson["isOnline"] = deviceShelly.hasInit;
//     devicesStateArray.add(deviceJson);
//   }

//   update["devices"] = devicesStateArray;
//   update["isScheduleOn"] = scheduleButton.getState();

//   JsonDocument updateResponseJson;
//   updateResponseJson["data"] = update;
//   updateResponseJson["status"] = "success";

//   String updateResponseString;
//   serializeJson(updateResponseJson, updateResponseString);

//   server.send(200, "application/json", updateResponseString);

//   return;
// }

// void handleGetConfig() {
//   if (!isAuthentified()) {
//     return;
//   }

//   File fileConfig = SD.open(FILE_CONFIG, "r");

//   if (!fileConfig) {
//     Serial.println("Could not open config.jso in [handleGetConfig]");
//   }

//   String configString;
//   while (fileConfig.available()) {
//     configString += fileConfig.readString();
//   }

//   JsonDocument configJson;
//   deserializeJson(configJson, configString);

//   JsonDocument getConfigResponseJson;
//   getConfigResponseJson["data"] = configJson;
//   getConfigResponseJson["status"] = "success";

//   String getConfigResponseString;
//   serializeJson(getConfigResponseJson, getConfigResponseString);

//   server.send(200, "application/json", getConfigResponseString);

//   return;
// }

// void handleGetScheduleState() {
//   if (!isAuthentified()) {
//     return;
//   }

//   JsonDocument getScheduleStateResponseJson;
//   getScheduleStateResponseJson["data"] = scheduleButton.getState();
//   getScheduleStateResponseJson["status"] = "success";

//   String getScheduleStateResponseString;
//   serializeJson(getScheduleStateResponseJson, getScheduleStateResponseString);

//   server.send(200, "application/json", getScheduleStateResponseString);

//   return;
// }

// void handleLogin() {
//   JsonDocument loginJson;
//   String loginString;
//   int statusCode = 401;

//   if (server.arg(0)) {
//     JsonDocument authBody = deserializePost(server.arg(0));

//     if (authBody["pass"] == serverPass) {
//       loginJson["status"] = "success";
//       loginJson["data"] = "Log in success !";
//       statusCode = 200;
//     } else {
//       loginJson["status"] = "error";
//       loginJson["data"] = "Log in Failed";
//     }
//   } else {
//     loginJson["status"] = "error";
//     loginJson["data"] = "No credentials provided !";
//   }

//   serializeJson(loginJson, loginString);
//   server.send(200, "application/json", loginString);

//   return;
// }

// void handlePing() {
//   if (!isAuthentified()) {
//     return;
//   }

//   JsonDocument pingJson;
//   pingJson["status"] = "success";
//   pingJson["data"] = "Online and authentified !";

//   String pingString;
//   serializeJson(pingJson, pingString);
//   server.send(200, "application/json", pingString);

//   return;
// }

// void handleRestart() {
//   if (!isAuthentified()) {
//     return;
//   }

//   JsonDocument restartJson;
//   restartJson["status"] = "success";
//   restartJson["data"] = "Restarting...";

//   String restartString;
//   serializeJson(restartJson, restartString);
//   server.send(200, "application/json", restartString);

//   ESP.restart();

//   return;
// }

// void ServerTaskCode(void* pvParameters) {
//   Serial.print("ServerTask running on core ");
//   Serial.println(xPortGetCoreID());

//   if (MDNS.begin("esp32")) {
//     Serial.println("MDNS responder started");
//   }

//   Serial.print("Server address: ");
//   Serial.println(WiFi.localIP());

//   server.enableCORS();

//   server.on("/ping", HTTP_GET, handlePing);
//   server.on("/login", HTTP_POST, handleLogin);
//   server.on("/updateconfig", HTTP_POST, handleUpdateConfig);
//   server.on("/last", HTTP_GET, handleLast);
//   server.on("/historical", HTTP_GET, handleHistorical);
//   server.on("/toggledevice", HTTP_GET, handleDeviceManualToggle);
//   server.on("/toggleschedule", HTTP_GET, handleScheduleManualToggle);
//   server.on("/getschedulestate", HTTP_GET, handleGetScheduleState);
//   server.on("/gethardwaretoggleupdate", HTTP_GET,
//             handleGetHardwareToggleUpdate);
//   server.on("/getconfig", HTTP_GET, handleGetConfig);
//   server.on("/getdevices", HTTP_GET, handleGetDevices);
//   server.on("/restart", HTTP_GET, handleRestart);

//   server.onNotFound(handleNotFound);

//   const char* headerKeys[] = {"User-Agent", "Cookie"};
//   size_t headerKeysSize = sizeof(headerKeys) / sizeof(char*);
//   server.collectHeaders(headerKeys, headerKeysSize);

//   server.begin();
//   Serial.println("HTTP server started");

//   for (;;) {
//     server.handleClient();
//     checkDevices();
//     checkScheduleButton();
//     activityLed.update();
//     delay(2);  // allow the cpu to switch to other tasks
//   }
// }

// void checkDevices() {
//   int updateMillis = false;

//   for (auto& [_, device] : devices) {
//     if (!scheduleButton.getState()) {
//       device.checkButton();
//     } else if (millis() - scheduleUpdateLastMillis > scheduleUpdatePeriode) {
//       device.checkSchedule();
//       // device.fetchShellyState();
//       updateMillis = true;
//     }
//   }

//   if (updateMillis) {
//     scheduleUpdateLastMillis = millis();
//   }
// }

// void checkScheduleButton() {
//   bool newState = scheduleButton.checkButton();
//   if (newState != scheduleButton.getState()) {
//     scheduleButton.setState(newState);
//   }
// }

// bool isAuthentified() {
//   return true;

//   if (server.hasHeader("Cookie")) {
//     String cookie = server.header("Cookie");

//     if (cookie.indexOf("sessionId=" + String(sessionId)) != -1) {
//       Serial.println("Authentification Successful");
//       return true;
//     }
//   }
//   Serial.println("Authentification Failed");

//   JsonDocument failedAuthJson;
//   failedAuthJson["status"] = "error";
//   failedAuthJson["data"] = "Not authentified !";

//   String failedAuthString;
//   serializeJson(failedAuthJson, failedAuthString);
//   server.send(403, "application/json", failedAuthString);

//   return false;
// }

// JsonDocument deserializePost(String body) {
//   JsonDocument jsonResponse;
//   DeserializationError err = deserializeJson(jsonResponse, body);

//   switch (err.code()) {
//     case DeserializationError::Ok:
//       break;
//     case DeserializationError::InvalidInput:
//       server.send(500, "text/plain", "Invalid input");
//       break;
//     case DeserializationError::NoMemory:
//       server.send(500, "text/plain", "Not enough memory");
//       break;
//     default:
//       server.send(500, "text/plain", "Deserialization failed");
//       break;
//   }

//   return jsonResponse;
// }
