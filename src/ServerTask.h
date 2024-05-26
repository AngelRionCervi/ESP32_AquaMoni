#pragma once
#include <ArduinoJson.h>
#include <SD.h>
#include <ESPmDNS.h>
#include "Device.h"
#include "utils.h"
#include "constants.h"
#include "global.h"
#include "secrets.h"

void loadConfig();

void handleLast();

void handleHistorical();

void handleUpdateConfig();

void handleNotFound();

void ServerTaskCode(void* pvParameters);

void checkDevices();