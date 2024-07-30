#pragma once
#include <WebServer.h>

#include <map>
#include <unordered_map>

#include "Device.h"
#include "ScheduleButton.h"
#include "ActivityLed.h"

#ifndef MY_GLOBALS_H
#define MY_GLOBALS_H

// This is a declaration of your variable, which tells the linker this value
// is found elsewhere.  Anyone who wishes to use it must include global.h,
// either directly or indirectly.
extern bool sensorError;
extern bool areSchedulesDisabled;
extern std::map<int, int> buttonMap;
extern std::map<int, int> ledMap;
extern std::unordered_map<std::string, Device> devices;
extern WiFiClient wifiClient;
extern WebServer server;
extern String wifiSSID;
extern String wifiPass;
extern String serverPass;
extern String sessionId;
extern ScheduleButton scheduleButton;
extern ActivityLed activityLed;
extern int scheduleUpdatePeriode;
extern int scheduleUpdateLastMillis;
extern int devicesStatesUpdatePeriode;
extern int devicesStatesUpdateLastMillis;
extern int measurementsUpdatePeriode;
extern int measurementsUpdateLastMillis;

#endif