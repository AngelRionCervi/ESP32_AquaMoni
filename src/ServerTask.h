#pragma once
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <SD.h>

#include "Device.h"
#include "constants.h"
#include "global.h"
#include "secrets.h"
#include "utils.h"

void handleLast();

void handleHistorical();

void handleUpdateConfig();

void handleNotFound();

void handleDeviceManualToggle();

void handleScheduleManualToggle();

void handleGetDevices();

void handleGetConfig();

void handleGetScheduleState();

void handleLogin();

void handlePing();

void handleRestart();

void ServerTaskCode(void* pvParameters);

void checkDevices();

bool isAuthentified();

JsonDocument deserializePost(String body);
