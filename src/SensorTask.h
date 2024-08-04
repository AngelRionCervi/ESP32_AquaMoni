#pragma once
#include <ArduinoJson.h>
#include <SD.h>
#include <ESPDateTime.h>
#include "PhMeasure.h"
#include "TempMeasure.h"
#include "utils.h"
#include "global.h"
#include "constants.h"


void SensorTaskCode(void* pvParameters);

void takeMeasurements(PhMeasure& phMeasure, TempMeasure& tempMeasure);

void writeToSd(float ph, float temp);

void writeToLast(JsonDocument* currentData);

void writeToHistorical(JsonDocument* currentData);

void balance_fetchDevicesStates();

void balance_checkForAutoScheduleOn();
