#pragma once
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#include <SD.h>

#include "Device.h"
#include "constants.h"
#include "global.h"
#include "secrets.h"
#include "utils.h"

void loadConfig();

void handleLast();

void handleHistorical();

void handleUpdateConfig();

void handleNotFound();

void handleDeviceManualToggle();

void handleScheduleManualToggle();

void handleGetDevicesState();

void handleGetConfig();

void handleGetScheduleState();

void ServerTaskCode(void* pvParameters);

void checkDevices();
