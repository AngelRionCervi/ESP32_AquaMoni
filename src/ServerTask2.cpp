#include "ServerTask2.h"

String handShakeType = "box_handshake";
String initType = "box_init";
String updateConfigType = "box_update_config";
String deviceToggleType = "box_device_manual_toggle";
String scheduleToggleType = "box_schedule_toggle";
String getDevicesInfoType = "box_get_devices_infos";
String getConfigType = "box_get_config";
String getScheduleStateType = "box_get_schedule_state";
String pingType = "box_ping";
String restartType = "box_restart";
String monitoringGetLastType = "box_monitoring_get_last";
String monitoringGetHistoricalType = "box_monitoring_get_historical";
String startHistoricalType = "box_start_historical";
String endHistoricalType = "box_end_historical";
String historicalDataStreamType = "box_hds";

void messageToMethods(String message) {
  JsonDocument messageJson;
  deserializeJson(messageJson, message);

  String type = messageJson["type"];
  JsonVariant dataJson = messageJson["data"];

  if (type == handShakeType) {
    sendHandShake();
  } else if (type == initType) {
    sendInitBox();
  } else if (type == updateConfigType) {
    handleUpdateConfig(dataJson);
  } else if (type == deviceToggleType) {
    handleDeviceManualToggle(dataJson);
  } else if (type == scheduleToggleType) {
    handleScheduleManualToggle();
  } else if (type == getDevicesInfoType) {
    handleGetDevices();
  } else if (type == getConfigType) {
    handleGetConfig();
  } else if (type == getScheduleStateType) {
    handleGetScheduleState();
  } else if (type == pingType) {
    handlePing();
  } else if (type == restartType) {
    handleRestart();
  } else if (type == monitoringGetLastType) {
    handleMonitoringGetLast();
  } else if (type == monitoringGetHistoricalType) {
    handleMonitoringGetHistorical(dataJson);
  } else {
    sendError("Unknown call type: ", type);
  }
}

void sendMessage(String message) {
  Serial.println("Sending message: ");
  Serial.println(message);
  webSocket.sendTXT(message);
}

void sendSuccess(JsonDocument& successJson) {
  successJson["status"] = "success";
  successJson["source"] = "box";
  successJson["boxId"] = boxId;
  String successString;
  serializeJson(successJson, successString);

  sendMessage(successString);
}

void sendError(String errorMessage, String type) {
  JsonDocument errorJson;
  String errorString;

  errorJson["status"] = "error";
  errorJson["type"] = type;
  errorJson["data"] = errorMessage;
  errorJson["source"] = "box";
  errorJson["boxId"] = boxId;

  serializeJson(errorJson, errorString);
  sendMessage(errorString);
}

void sendError(String errorMessage, String type, JsonDocument& info) {
  JsonDocument errorJson;
  String errorString;

  errorJson["status"] = "error";
  errorJson["type"] = type;
  errorJson["data"] = errorMessage;
  errorJson["source"] = "box";
  errorJson["info"] = info;
  errorJson["boxId"] = boxId;

  serializeJson(errorJson, errorString);
  sendMessage(errorString);
}

void sendHandShake() {
  JsonDocument handShakeJson;
  handShakeJson["data"] = boxId;
  handShakeJson["type"] = handShakeType;

  sendSuccess(handShakeJson);
}

void sendInitBox() {
  // JsonDocument initBoxJson;

  handleGetConfig();
  handleGetDevices();
  handleGetScheduleState();

  // initBoxJson["data"] = "1234";  // box config + devices states
  // initBoxJson["type"] = initType;

  // sendSuccess(initBoxJson);
}

void handleUpdateConfig(JsonVariant newConfigJson) {
  String type = updateConfigType;
  File fileConfig = SD.open(FILE_CONFIG, "w");

  if (!fileConfig) {
    sendError("Could not open config.jso [handleUpdateConfig]", type);
    return;
  }

  serializeJson(newConfigJson, fileConfig);
  fileConfig.close();

  JsonDocument updatedJson;
  updatedJson["data"] = "Config updated, resetting...";
  updatedJson["type"] = type;

  sendSuccess(updatedJson);

  return;
}

void handleDeviceManualToggle(String deviceId) {
  if (scheduleButton.getState()) {
    return;
  }

  if (devices.count(deviceId.c_str()) == 0) {
    String errorMessage = "Device not found [handleDeviceManualToggle]";
    Serial.println(errorMessage);
    JsonDocument dataJson;
    dataJson["id"] = deviceId;
    sendError(errorMessage, deviceToggleType, dataJson);
    return;
  }

  Device& device = devices.at(deviceId.c_str());

  device.toggleShellyState();
  bool newState = device.shellyState;

  JsonDocument dataJson;
  dataJson["id"] = deviceId;
  dataJson["state"] = newState;

  JsonDocument donePayloadJson;
  donePayloadJson["data"] = dataJson;
  donePayloadJson["type"] = deviceToggleType;

  sendSuccess(donePayloadJson);

  return;
}

void handleScheduleManualToggle() {
  scheduleButton.toggleState();

  JsonDocument donePayloadJson;
  donePayloadJson["data"] = scheduleButton.getState();
  donePayloadJson["type"] = scheduleToggleType;

  sendSuccess(donePayloadJson);

  return;
}

void handleGetDevices() {
  JsonDocument devicesJson;
  JsonArray devicesStateArray = devicesJson.to<JsonArray>();

  for (auto& [id, device] : devices) {
    JsonDocument deviceJson;
    ShellyPlug deviceShelly = device.getShellyInfo();
    deviceJson["name"] = device.name;
    deviceJson["id"] = id;
    deviceJson["state"] = deviceShelly.state;
    deviceJson["isOnline"] = deviceShelly.hasInit;
    devicesStateArray.add(deviceJson);
  }

  JsonDocument updateResponseJson;
  updateResponseJson["data"] = devicesStateArray;
  updateResponseJson["type"] = getDevicesInfoType;

  sendSuccess(updateResponseJson);

  return;
}

void handleGetConfig() {
  String type = getConfigType;
  File fileConfig = SD.open(FILE_CONFIG, "r");

  if (!fileConfig) {
    String errorMessage = "Could not open config.jso in [handleGetConfig]";
    Serial.println(errorMessage);
    sendError(errorMessage, type);
    return;
  }

  String configString;
  while (fileConfig.available()) {
    configString += fileConfig.readString();
  }

  JsonDocument configJson;
  deserializeJson(configJson, configString);

  JsonDocument getConfigResponseJson;
  getConfigResponseJson["data"] = configJson;
  getConfigResponseJson["type"] = type;

  sendSuccess(getConfigResponseJson);

  return;
}

void handleGetScheduleState() {
  String type = getScheduleStateType;
  JsonDocument getScheduleStateResponseJson;
  getScheduleStateResponseJson["data"] = scheduleButton.getState();
  getScheduleStateResponseJson["type"] = type;

  sendSuccess(getScheduleStateResponseJson);

  return;
}

void handlePing() {
  String type = pingType;
  JsonDocument pingJson;
  pingJson["data"] = "Online and authentified !";
  pingJson["type"] = type;

  sendSuccess(pingJson);

  return;
}

void handleRestart() {
  String type = restartType;
  JsonDocument restartJson;
  restartJson["data"] = "Restarting...";
  restartJson["type"] = type;

  sendSuccess(restartJson);

  delay(1000);

  ESP.restart();

  return;
}

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.println("[WSc] Disconnected!");
      break;
    case WStype_CONNECTED:
      Serial.print("[WSc] Connected to url: ");
      Serial.println((char*)payload);

      sendHandShake();
      break;
    case WStype_TEXT:
      Serial.print("[WSc] get text: ");
      Serial.println((char*)payload);

      messageToMethods((char*)payload);
      break;
    case WStype_BIN:
      Serial.print("[WSc] get binary, length: ");
      Serial.println(length);

      break;
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
  }
}

void ServerTaskCode2(void* pvParameters) {
  Serial.println("Connecting to websocket server");
  // for prod:
  // webSocket.begin("dash.aqua-dash.com", 80, "/websocket");
  // for local :
  webSocket.begin("192.168.1.18", 3000, "/websocket");
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);

  for (;;) {
    webSocket.loop();
    int millisNow = millis();
    if (millisNow - sendLastMeasurementsMillis >
        sendLastMeasurementsUpdatePeriode) {
      handleMonitoringGetLast();
      sendLastMeasurementsMillis = millisNow;
    }
    delay(2);
  }
}

void handleMonitoringGetLast() {
  String type = monitoringGetLastType;

  File fileLast = SD.open(FILE_LAST, "r");

  if (!fileLast) {
    String errorMessage = "Could not open last.jso for sending";
    Serial.println(errorMessage);
    sendError(errorMessage, type);
    activityLed.setState("errorBlink");
  }

  String content;
  while (fileLast.available()) {
    content += fileLast.readString();
  }

  JsonDocument lastJson;
  deserializeJson(lastJson, content);

  JsonDocument getLastResponseJson;
  getLastResponseJson["data"] = lastJson;
  getLastResponseJson["type"] = type;

  sendSuccess(getLastResponseJson);

  fileLast.close();
}

void handleMonitoringGetHistorical(String totalDays) {
  String type = monitoringGetHistoricalType;

  int daysLength;

  String* days = splitString(totalDays, ',', daysLength);

  if (daysLength > 62) {
    String errorMessage = "Too many days requested [handleMonitoringGetHistorical]";
    Serial.println(errorMessage);
    sendError(errorMessage, type);

    return;
  }

  sendMessage(startHistoricalType);

  for (int i = 0; i < daysLength; i++) {
    String fileName = "/historical/";
    fileName += days[i];
    fileName += ".jso";

    Serial.println("fileName");
    Serial.println(fileName);

    File dayFile = SD.open(fileName, "r");

    if (!dayFile) {
      String errorMsg = "Could not open: " + String(fileName);
      sendError(errorMsg, type);
      Serial.println(errorMsg);
      dayFile.close();
    } else {
      String content = historicalDataStreamType + "_";

      while (dayFile.available()) {
        content += dayFile.readString();
      }

      sendMessage(content);

      dayFile.close();
    }
  }

  sendMessage(endHistoricalType);

  delete[] days;

  return;
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