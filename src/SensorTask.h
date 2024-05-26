#pragma once
#include <ArduinoJson.h>
#include <SD.h>
#include <ESPDateTime.h>
#include "PhMesure.h"
#include "TempMesure.h"
#include "utils.h"
#include "global.h"
#include "constants.h"


void SensorTaskCode(void* pvParameters);

void writeToSd(float ph, float temp);

void writeToLast(JsonDocument* currentData);

void writeToHistorical(JsonDocument* currentData);
