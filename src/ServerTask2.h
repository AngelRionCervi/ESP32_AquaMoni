#pragma once
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <SD.h>

#include "Device.h"
#include "constants.h"
#include "global.h"
#include "secrets.h"
#include "utils.h"

void checkForMessage();

void sendMessage(String message);

void sendSuccess(JsonDocument& successJson);

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length);

void sendError(String errorMessage, String type);

void sendHandShake();

void sendInitBox();

void handleUpdateConfig(JsonVariant newConfigJson);

void handleDeviceManualToggle(String deviceId);

void handleScheduleManualToggle();

void handleGetDevices();

void handleGetHardwareToggleUpdate();

void handleGetConfig();

void handleGetScheduleState();

void handlePing();

void handleRestart();

void ServerTaskCode2(void* pvParameters);

void handleLast();

void handleHistorical(String argValue);

JsonDocument deserializePost(String body);
