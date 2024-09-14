#pragma once
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <SD.h>

#include "Device.h"
#include "constants.h"
#include "global.h"
#include "utils.h"

void sendMessage(String message);

void sendSuccess(JsonDocument& successJson);

void webSocketEvent(WStype_t type, uint8_t* payload, size_t length);

void sendError(String errorMessage, String type);

void sendError(String errorMessage, String type, JsonDocument& infos);

void sendHandShake();

void sendInitBox();

void handleUpdateConfig(JsonVariant newConfigJson);

void handleDeviceManualToggle(String deviceId);

void handleScheduleManualToggle();

void handleGetDevices();

void handleGetConfig();

void handleGetScheduleState();

void handlePing();

void handleRestart();

void ServerTaskCode2(void* pvParameters);

void handleMonitoringGetLastHistoricalUpdate();

void handleMonitoringGetLive();

void handleMonitoringGetHistorical(String argValue);

JsonDocument deserializePost(String body);
