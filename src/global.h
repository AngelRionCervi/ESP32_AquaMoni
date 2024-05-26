#include <WebServer.h>

#include <map>
#include <unordered_map>

#include "Device.h"

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
extern int scheduleUpdatePeriode;
extern int scheduleUpdateLastMillis;
extern void (*resetFunc)(void);

#endif